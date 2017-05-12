# include "HelloWorldScene.h"
# include "ui/CocosGUI.h"
# include "Monster.h"
# define database UserDefault::getInstance()
#pragma execution_character_set("utf-8")
USING_NS_CC;
using namespace cocos2d::ui;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();

    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();
	
	// ���ñ���
	TMXTiledMap* tmx = TMXTiledMap::create("map.tmx");
	tmx->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	tmx->setAnchorPoint(Vec2(0.5, 0.5));
	tmx->setScale(Director::getInstance()->getContentScaleFactor());
	this->addChild(tmx, 0);
	// auto bg = Sprite::create("background.jpg");
	// bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	// this->addChild(bg, 0);

	// �����÷�ͳ��
	time = Label::createWithTTF("0", "fonts/arial.ttf", 36);
	time->setPosition(Vec2(origin.x+visibleSize.width/2, origin.y+visibleSize.height-30));
	this->addChild(time, 1);
	schedule(schedule_selector(HelloWorld::update), 0.5);
	dtime = 0;

	// ��ʼ��Ϊ��ִ�У���������ʼִ��ʱ��isDone��ֵΪfalse��������ִ�н���ʱ��isDone��ֵΪtrue
	isDone = true;

	// ����һ����ͼ
	auto texture = Director::getInstance()->getTextureCache()->addImage("$lucia_2.png");
	// ����ͼ�������ص�λ�и�����ؼ�֡
	auto frame0 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 113, 113)));
	// ʹ�õ�һ֡��������
	player = Sprite::createWithSpriteFrame(frame0);
	player->setPosition(Vec2(origin.x + visibleSize.width / 2,
							origin.y + visibleSize.height/2));
	addChild(player, 3);

	// hp��
	Sprite* sp0 = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
	Sprite* sp = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));

	// ʹ��hp������progressBar
	pT = ProgressTimer::create(sp);
	pT->setScaleX(90);
	pT->setAnchorPoint(Vec2(0, 0));
	pT->setType(ProgressTimerType::BAR);
	pT->setBarChangeRate(Point(1, 0));
	pT->setMidpoint(Point(0, 1));
	pT->setPercentage(100);
	pT->setPosition(Vec2(origin.x+14*pT->getContentSize().width,origin.y + visibleSize.height - 2*pT->getContentSize().height));
	addChild(pT,1);
	sp0->setAnchorPoint(Vec2(0, 0));
	sp0->setPosition(Vec2(origin.x + pT->getContentSize().width, origin.y + visibleSize.height - sp0->getContentSize().height));
	addChild(sp0,0);

	// ���ɹ���
	schedule(schedule_selector(HelloWorld::createMonster), 5);

	// ��ײ���
	schedule(schedule_selector(HelloWorld::detectHit), 0.2);

	// ��̬����
	idle.reserve(1);
	idle.pushBack(frame0);

	// ��������
	attack.reserve(17);
	for (int i = 0; i < 17; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(113*i,0,113,113)));
		attack.pushBack(frame);
	}
	auto attack_animation = Animation::createWithSpriteFrames(attack, 0.1f);
	attack_animation->setRestoreOriginalFrame(true);
	AnimationCache::getInstance()->addAnimation(attack_animation, "attackAnimation");

	// ���Է��չ�������
	// ��������(֡����22֡���ߣ�90����79��
	auto texture2 = Director::getInstance()->getTextureCache()->addImage("$lucia_dead.png");
	dead.reserve(22);
	for (int i = 0; i < 22; i++) {
		auto frame = SpriteFrame::createWithTexture(texture2, CC_RECT_PIXELS_TO_POINTS(Rect(79*i, 0, 90, 79)));
		dead.pushBack(frame);
	}
	auto dead_animation = Animation::createWithSpriteFrames(dead, 0.1f);
	dead_animation->setRestoreOriginalFrame(true);
	AnimationCache::getInstance()->addAnimation(dead_animation, "deadAnimation");

	// �˶�����(֡����8֡���ߣ�101����68��
	auto texture3 = Director::getInstance()->getTextureCache()->addImage("$lucia_forward.png");
	run.reserve(8);
	for (int i = 0; i < 8; i++) {
		auto frame = SpriteFrame::createWithTexture(texture3, CC_RECT_PIXELS_TO_POINTS(Rect(68 * i, 0, 68, 101)));
		run.pushBack(frame);
	}
	auto run_animation = Animation::createWithSpriteFrames(run, 0.1f);
	//run_animation->setRestoreOriginalFrame(true);
	AnimationCache::getInstance()->addAnimation(run_animation, "runAnimation");

	// �����
	auto buttonW = Button::create("W.png", "W.png");
	auto buttonS = Button::create("S.png", "S.png");
	auto buttonA = Button::create("A.png", "A.png");
	auto buttonD = Button::create("D.png", "D.png");
	buttonW->setPosition(Vec2(origin.x + 60, origin.y + 70));
	buttonS->setPosition(Vec2(origin.x + 60, origin.y + 20));
	buttonA->setPosition(Vec2(origin.x + 20, origin.y + 20));
	buttonD->setPosition(Vec2(origin.x + 100, origin.y + 20));

	// W���¼�������
	buttonW->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		switch (type) {
		case ui::Widget::TouchEventType::BEGAN:
			//schedule(schedule_selector(HelloWorld::moveW), 0.3f);
			break;
		case ui::Widget::TouchEventType::ENDED:
			//unschedule(schedule_selector(HelloWorld::moveW));
			moveW(-1);
			break;
		}
	});

	// S���¼�������
	buttonS->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		switch (type) {
		case ui::Widget::TouchEventType::BEGAN:
			//schedule(schedule_selector(HelloWorld::moveS), 0.3f);
			break;
		case ui::Widget::TouchEventType::ENDED:
			//unschedule(schedule_selector(HelloWorld::moveS));
			moveS(-1);
			break;
		}
	});

	// A���¼�������
	buttonA->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		switch (type) {
		case ui::Widget::TouchEventType::BEGAN:
			//schedule(schedule_selector(HelloWorld::moveA), 0.3f);
			break;
		case ui::Widget::TouchEventType::ENDED:
			//unschedule(schedule_selector(HelloWorld::moveA));
			moveA(-1);
			break;
		}
	});

	// D���¼�������
	buttonD->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		switch (type) {
		case ui::Widget::TouchEventType::BEGAN:
			//schedule(schedule_selector(HelloWorld::moveD), 0.3f);
			break;
		case ui::Widget::TouchEventType::ENDED:
			//unschedule(schedule_selector(HelloWorld::moveD));
			moveD(-1);
			break;
		}
	});

	this->addChild(buttonW);
	this->addChild(buttonS);
	this->addChild(buttonA);
	this->addChild(buttonD);

	// ���ܼ�
	auto buttonY = Button::create("Y.png", "Y.png");
	buttonY->setPosition(Vec2(origin.x + visibleSize.width-50, origin.y + 20));

	// ���ܼ�Y���¼�������
	buttonY->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		switch (type) {
		case ui::Widget::TouchEventType::BEGAN:
			break;
		case ui::Widget::TouchEventType::ENDED:
			if (isDone == false) return;
			else isDone = false;
			/*Animate* attackAnimation = Animate::create(AnimationCache::getInstance()->getAnimation("attackAnimation"));
			player->runAction(attackAnimation);*/

			auto callFunc = CallFunc::create([&] {isDone = true; });
			auto seq = Sequence::createWithTwoActions(Animate::create(AnimationCache::getInstance()->getAnimation("attackAnimation")),
				callFunc);
			player->runAction(seq);

			Rect playerRect = player->getBoundingBox();
			Rect attackRect = Rect(playerRect.getMinX() - 40, playerRect.getMinY(),
				playerRect.getMaxX() - playerRect.getMinX() + 80,
				playerRect.getMaxY() - playerRect.getMinY());
			auto factory = Factory::getInstance();
			Sprite* collision = factory->collider(attackRect);
			if (collision != NULL) {
				Animate* monsterDead = Animate::create(AnimationCache::getInstance()->getAnimation("monsterDeadAnimation"));
				//collision->runAction(monsterDead);
				CCActionInterval* fadeout = CCFadeOut::create(2);
				//collision->runAction(fadeout);
				auto seq = Sequence::createWithTwoActions(monsterDead, fadeout);
				collision->runAction(seq);
				factory->removeMonster(collision);
				dtime++;
				database->setIntegerForKey("killNum", dtime);
				// ��set������ݲ������ϱ��浽XML�ļ��С�����һ��Ҫ�ǵ���flush()���������ݣ�����ᶪʧ
				database->flush();
				log("%s", FileUtils::getInstance()->getWritablePath().c_str());
				if (pT->getPercentage() + 40 <= 100) {
					pT->setPercentage(pT->getPercentage() + 40);
				}
				else {
					pT->setPercentage(100);
				}
			}

			break;
		}
	});
	//this->addChild(buttonX);
	this->addChild(buttonY);
	return true;
}

// �����D���ƶ�
void HelloWorld::moveD(float dt) {
	//auto seq = Sequence::createWithTwoActions(Animate::create(AnimationCache::getInstance()->getAnimation("runAnimation")),
		//                                      MoveBy::create(0.15f, Vec2(10, 0)));
	player->setFlipX(false);
	auto location = player->getPosition();
	if (location.x + 20 >= visibleSize.width) {
		//auto seq = Sequence::createWithTwoActions(MoveBy::create(0.6f, Vec2(visibleSize.width-location.x-10, 0)),
			//Animate::create(AnimationCache::getInstance()->getAnimation("runAnimation")));
		//player->runAction(seq);
		player->runAction(Animate::create(AnimationCache::getInstance()->getAnimation("runAnimation")));
		player->runAction(MoveBy::create(0.6f, Vec2(visibleSize.width - location.x - 10, 0)));
		return;
	}
	//auto seq = Sequence::createWithTwoActions(MoveBy::create(0.6f, Vec2(20, 0)),
		//Animate::create(AnimationCache::getInstance()->getAnimation("runAnimation")));
	//player->runAction(seq);
	player->runAction(Animate::create(AnimationCache::getInstance()->getAnimation("runAnimation")));
	player->runAction(MoveBy::create(0.6f, Vec2(20, 0)));
}

// �����A���ƶ�
void HelloWorld::moveA(float dt) {
	//auto seq = Sequence::createWithTwoActions(Animate::create(AnimationCache::getInstance()->getAnimation("runAnimation")),
		//MoveBy::create(0.15f, Vec2(-10, 0)));
	player->setFlipX(true);
	auto location = player->getPosition();
	if (location.x - 20 <= 0) {
		//auto seq = Sequence::createWithTwoActions(MoveBy::create(0.6f, Vec2(-location.x+10, 0)),
			//Animate::create(AnimationCache::getInstance()->getAnimation("runAnimation")));
		//player->runAction(seq);
		player->runAction(Animate::create(AnimationCache::getInstance()->getAnimation("runAnimation")));
		player->runAction(MoveBy::create(0.6f, Vec2(-location.x + 10, 0)));
		return;
	}
	//auto seq = Sequence::createWithTwoActions(MoveBy::create(0.6f, Vec2(-20, 0)),
		//Animate::create(AnimationCache::getInstance()->getAnimation("runAnimation")));
	//player->runAction(seq);
	player->runAction(Animate::create(AnimationCache::getInstance()->getAnimation("runAnimation")));
	player->runAction(MoveBy::create(0.6f, Vec2(-20, 0)));
}

// �����W���ƶ�
void HelloWorld::moveW(float dt) {
	//auto seq = Sequence::createWithTwoActions(Animate::create(AnimationCache::getInstance()->getAnimation("runAnimation")),
		//MoveBy::create(0.15f, Vec2(0, 10)));
	auto location = player->getPosition();
	if (location.y + 20 >= visibleSize.height) {
		//auto seq = Sequence::createWithTwoActions(MoveBy::create(0.6f, Vec2(0, visibleSize.height-location.y-10)),
			//Animate::create(AnimationCache::getInstance()->getAnimation("runAnimation")));
		//player->runAction(seq);
		player->runAction(Animate::create(AnimationCache::getInstance()->getAnimation("runAnimation")));
		player->runAction(MoveBy::create(0.6f, Vec2(0, visibleSize.height - location.y - 10)));
		return;
	}
	//auto seq = Sequence::createWithTwoActions(MoveBy::create(0.6f, Vec2(0, 20)),
		//Animate::create(AnimationCache::getInstance()->getAnimation("runAnimation")));
	//player->runAction(seq);
	player->runAction(Animate::create(AnimationCache::getInstance()->getAnimation("runAnimation")));
	player->runAction(MoveBy::create(0.6f, Vec2(0, 20)));
}

// �����S���ƶ�
void HelloWorld::moveS(float dt) {
	//auto seq = Sequence::createWithTwoActions(Animate::create(AnimationCache::getInstance()->getAnimation("runAnimation")),
		//MoveBy::create(0.15f, Vec2(0, -10)));
	auto location = player->getPosition();
	if (location.y - 20 <= 0) {
		//auto seq = Sequence::createWithTwoActions(MoveBy::create(0.6f, Vec2(0, -location.y+10)),
			//Animate::create(AnimationCache::getInstance()->getAnimation("runAnimation")));
		//player->runAction(seq);
		player->runAction(Animate::create(AnimationCache::getInstance()->getAnimation("runAnimation")));
		player->runAction(MoveBy::create(0.6f, Vec2(0, -location.y + 10)));
		return;
	}
	//auto seq = Sequence::createWithTwoActions(MoveBy::create(0.6f, Vec2(0, -20)),
		//Animate::create(AnimationCache::getInstance()->getAnimation("runAnimation")));
	//player->runAction(seq);
	player->runAction(Animate::create(AnimationCache::getInstance()->getAnimation("runAnimation")));
	player->runAction(MoveBy::create(0.6f, Vec2(0, -20)));
}

// �¼����º���
void HelloWorld::update(float delta) {
	char str[10];
	sprintf(str, "%d", dtime); // ��int����ת��Ϊ�ַ���char*����
	time->setString(str);
}

// ���ɹ���
void HelloWorld::createMonster(float delta) {
	auto factory = Factory::getInstance();
	auto monster = factory->createMonster();
	monster->setPosition(random(origin.x, origin.x + visibleSize.width), random(origin.y, origin.y + visibleSize.height));
	this->addChild(monster, 3);
	factory->moveMonster(player->getPosition(), 5);
}

// �����ײ
// getBoundingBox()���ڻ�ȡ�����������
void HelloWorld::detectHit(float delta) {
	auto factory = Factory::getInstance();
	Sprite* collision = factory->collider(player->getBoundingBox());
	if (collision != NULL) {
		buttonX();
		Animate* monsterDead = Animate::create(AnimationCache::getInstance()->getAnimation("monsterDeadAnimation"));
		collision->runAction(monsterDead);
		//collision->setVisible(false);
		// ��������������ʧ
		CCActionInterval* fadeout = CCFadeOut::create(2);
		collision->runAction(fadeout);
		factory->removeMonster(collision);
	}
}

void HelloWorld::buttonX() {
	if (isDone == false) return; // ����δ��ɣ�����ִ���¶���
	else isDone = false; // ��ʼִ�ж���
	if (pT->getPercentage() == 0) return; // ������Ϊ0ʱ������ִ�иö���
	auto callFunc = CallFunc::create([&] {isDone = true; }); // ���嶯��ִ����ϵĻص�����
	/*Animate* deadAnimation = Animate::create(AnimationCache::getInstance()->getAnimation("deadAnimation"));
	auto action = Sequence::create(callFunc);*/
	auto seq = Sequence::createWithTwoActions(Animate::create(AnimationCache::getInstance()->getAnimation("deadAnimation")),
	callFunc);
	player->runAction(seq);
	//player->runAction(deadAnimation);
	if (pT->getPercentage() - 40 >= 0) { // ÿ��X�������ٽ�����40
	    pT->setPercentage(pT->getPercentage() - 40);
	}
	else {
	    pT->setPercentage(0);
	}
}