# include "HelloWorldScene.h"
# include "ui/CocosGUI.h"
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
	
	// 设置背景
	auto bg = Sprite::create("background.jpg");
	bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(bg, 0);

	// 创建倒计时
	time = Label::createWithTTF("120", "fonts/arial.ttf", 36);
	time->setPosition(Vec2(origin.x+visibleSize.width/2, origin.y+visibleSize.height-30));
	this->addChild(time);
	schedule(schedule_selector(HelloWorld::updateTime), 1);
	dtime = 120;

	// 初始化为可执行，当动画开始执行时，isDone的值为false，当动画执行结束时，isDone的值为true
	isDone = true;

	// 创建一张贴图
	auto texture = Director::getInstance()->getTextureCache()->addImage("$lucia_2.png");
	// 从贴图中以像素单位切割，创建关键帧
	auto frame0 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 113, 113)));
	// 使用第一帧创建精灵
	player = Sprite::createWithSpriteFrame(frame0);
	player->setPosition(Vec2(origin.x + visibleSize.width / 2,
							origin.y + visibleSize.height/2));
	addChild(player, 3);

	// hp条
	Sprite* sp0 = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
	Sprite* sp = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));

	// 使用hp条设置progressBar
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

	// 静态动画
	idle.reserve(1);
	idle.pushBack(frame0);

	// 攻击动画
	attack.reserve(17);
	for (int i = 0; i < 17; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(113*i,0,113,113)));
		attack.pushBack(frame);
	}
	auto attack_animation = Animation::createWithSpriteFrames(attack, 0.1f);
	attack_animation->setRestoreOriginalFrame(true);
	AnimationCache::getInstance()->addAnimation(attack_animation, "attackAnimation");

	// 可以仿照攻击动画
	// 死亡动画(帧数：22帧，高：90，宽：79）
	auto texture2 = Director::getInstance()->getTextureCache()->addImage("$lucia_dead.png");
	dead.reserve(22);
	for (int i = 0; i < 22; i++) {
		auto frame = SpriteFrame::createWithTexture(texture2, CC_RECT_PIXELS_TO_POINTS(Rect(79*i, 0, 90, 79)));
		dead.pushBack(frame);
	}
	auto dead_animation = Animation::createWithSpriteFrames(dead, 0.1f);
	dead_animation->setRestoreOriginalFrame(true);
	AnimationCache::getInstance()->addAnimation(dead_animation, "deadAnimation");

	// 运动动画(帧数：8帧，高：101，宽：68）
	auto texture3 = Director::getInstance()->getTextureCache()->addImage("$lucia_forward.png");
	run.reserve(8);
	for (int i = 0; i < 8; i++) {
		auto frame = SpriteFrame::createWithTexture(texture3, CC_RECT_PIXELS_TO_POINTS(Rect(68 * i, 0, 68, 101)));
		run.pushBack(frame);
	}
	auto run_animation = Animation::createWithSpriteFrames(run, 0.1f);
	run_animation->setRestoreOriginalFrame(true);
	AnimationCache::getInstance()->addAnimation(run_animation, "runAnimation");

	// 方向键
	auto buttonW = Button::create("W.png", "W.png");
	auto buttonS = Button::create("S.png", "S.png");
	auto buttonA = Button::create("A.png", "A.png");
	auto buttonD = Button::create("D.png", "D.png");
	buttonW->setPosition(Vec2(origin.x + 60, origin.y + 60));
	buttonS->setPosition(Vec2(origin.x + 60, origin.y + 20));
	buttonA->setPosition(Vec2(origin.x + 20, origin.y + 20));
	buttonD->setPosition(Vec2(origin.x + 100, origin.y + 20));

	// W键事件处理函数
	buttonW->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		switch (type) {
		case ui::Widget::TouchEventType::BEGAN:
			schedule(schedule_selector(HelloWorld::moveW), 0.3f);
			break;
		case ui::Widget::TouchEventType::ENDED:
			unschedule(schedule_selector(HelloWorld::moveW));
			break;
		}
	});

	// S键事件处理函数
	buttonS->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		switch (type) {
		case ui::Widget::TouchEventType::BEGAN:
			schedule(schedule_selector(HelloWorld::moveS), 0.3f);
			break;
		case ui::Widget::TouchEventType::ENDED:
			unschedule(schedule_selector(HelloWorld::moveS));
			break;
		}
	});

	// A键事件处理函数
	buttonA->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		switch (type) {
		case ui::Widget::TouchEventType::BEGAN:
			schedule(schedule_selector(HelloWorld::moveA), 0.3f);
			break;
		case ui::Widget::TouchEventType::ENDED:
			unschedule(schedule_selector(HelloWorld::moveA));
			break;
		}
	});

	// D键事件处理函数
	buttonD->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		switch (type) {
		case ui::Widget::TouchEventType::BEGAN:
			schedule(schedule_selector(HelloWorld::moveD), 0.3f);
			break;
		case ui::Widget::TouchEventType::ENDED:
			unschedule(schedule_selector(HelloWorld::moveD));
			break;
		}
	});

	this->addChild(buttonW);
	this->addChild(buttonS);
	this->addChild(buttonA);
	this->addChild(buttonD);

	// 技能键
	auto buttonX = Button::create("X.png", "X.png");
	auto buttonY = Button::create("Y.png", "Y.png");
	buttonX->setPosition(Vec2(origin.x + visibleSize.width-50, origin.y + 60));
	buttonY->setPosition(Vec2(origin.x + visibleSize.width-50, origin.y + 20));

	// 技能键X的事件处理函数
	buttonX->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		switch (type) {
		case ui::Widget::TouchEventType::BEGAN:
			break;
		case ui::Widget::TouchEventType::ENDED:
			if (isDone == false) return; // 动画未完成，不能执行新动画
			else isDone = false; // 开始执行动画
			if (pT->getPercentage() == 0) return; // 进度条为0时不可再执行该动画
			auto callFunc = CallFunc::create([&] {isDone = true; }); // 定义动画执行完毕的回调函数
			/*Animate* deadAnimation = Animate::create(AnimationCache::getInstance()->getAnimation("deadAnimation"));
			auto action = Sequence::create(callFunc);*/
			auto seq = Sequence::createWithTwoActions(Animate::create(AnimationCache::getInstance()->getAnimation("deadAnimation")),
				callFunc);
			player->runAction(seq);
			//player->runAction(deadAnimation);
			if (pT->getPercentage() - 40 >= 0) { // 每次X操作减少进度条40
				pT->setPercentage(pT->getPercentage() - 40);
			}
			else {
				pT->setPercentage(0);
			}
			break;
		}
	});

	// 技能键Y的事件处理函数
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
			if (pT->getPercentage() + 40 <= 100) {
				pT->setPercentage(pT->getPercentage() + 40);
			}
			else {
				pT->setPercentage(100);
			}
			break;
		}
	});
	this->addChild(buttonX);
	this->addChild(buttonY);
	return true;
}

// 方向键D的移动
void HelloWorld::moveD(float dt) {
	//auto seq = Sequence::createWithTwoActions(Animate::create(AnimationCache::getInstance()->getAnimation("runAnimation")),
		//                                      MoveBy::create(0.15f, Vec2(10, 0)));
	auto location = player->getPosition();
	if (location.x + 20 >= visibleSize.width) {
		auto seq = Sequence::createWithTwoActions(MoveBy::create(0.6f, Vec2(visibleSize.width-location.x-10, 0)),
			Animate::create(AnimationCache::getInstance()->getAnimation("runAnimation")));
		player->runAction(seq);
		return;
	}
	auto seq = Sequence::createWithTwoActions(MoveBy::create(0.6f, Vec2(20, 0)),
		Animate::create(AnimationCache::getInstance()->getAnimation("runAnimation")));
	player->runAction(seq);
}

// 方向键A的移动
void HelloWorld::moveA(float dt) {
	//auto seq = Sequence::createWithTwoActions(Animate::create(AnimationCache::getInstance()->getAnimation("runAnimation")),
		//MoveBy::create(0.15f, Vec2(-10, 0)));
	auto location = player->getPosition();
	if (location.x - 20 <= 0) {
		auto seq = Sequence::createWithTwoActions(MoveBy::create(0.6f, Vec2(-location.x+10, 0)),
			Animate::create(AnimationCache::getInstance()->getAnimation("runAnimation")));
		player->runAction(seq);
		return;
	}
	auto seq = Sequence::createWithTwoActions(MoveBy::create(0.6f, Vec2(-20, 0)),
		Animate::create(AnimationCache::getInstance()->getAnimation("runAnimation")));
	player->runAction(seq);
}

// 方向键W的移动
void HelloWorld::moveW(float dt) {
	//auto seq = Sequence::createWithTwoActions(Animate::create(AnimationCache::getInstance()->getAnimation("runAnimation")),
		//MoveBy::create(0.15f, Vec2(0, 10)));
	auto location = player->getPosition();
	if (location.y + 20 >= visibleSize.height) {
		auto seq = Sequence::createWithTwoActions(MoveBy::create(0.6f, Vec2(0, visibleSize.height-location.y-10)),
			Animate::create(AnimationCache::getInstance()->getAnimation("runAnimation")));
		player->runAction(seq);
		return;
	}
	auto seq = Sequence::createWithTwoActions(MoveBy::create(0.6f, Vec2(0, 20)),
		Animate::create(AnimationCache::getInstance()->getAnimation("runAnimation")));
	player->runAction(seq);
}

// 方向键S的移动
void HelloWorld::moveS(float dt) {
	//auto seq = Sequence::createWithTwoActions(Animate::create(AnimationCache::getInstance()->getAnimation("runAnimation")),
		//MoveBy::create(0.15f, Vec2(0, -10)));
	auto location = player->getPosition();
	if (location.y - 20 <= 0) {
		auto seq = Sequence::createWithTwoActions(MoveBy::create(0.6f, Vec2(0, -location.y+10)),
			Animate::create(AnimationCache::getInstance()->getAnimation("runAnimation")));
		player->runAction(seq);
		return;
	}
	auto seq = Sequence::createWithTwoActions(MoveBy::create(0.6f, Vec2(0, -20)),
		Animate::create(AnimationCache::getInstance()->getAnimation("runAnimation")));
	player->runAction(seq);
}

// 事件更新函数
void HelloWorld::updateTime(float delta) {
	dtime--;
	if (dtime < 0) dtime = 0;
	char str[10];
	sprintf(str, "%d", dtime); // 将int类型转化为字符串char*类型
	time->setString(str);
}
