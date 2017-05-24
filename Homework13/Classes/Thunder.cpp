#include "Thunder.h"
#include "SimpleAudioEngine.h"
#include <algorithm>

USING_NS_CC;

using namespace CocosDenshion;

Scene* Thunder::createScene() {
  // 'scene' is an autorelease object
  auto scene = Scene::create();

  // 'layer' is an autorelease object
  auto layer = Thunder::create();

  // add layer as a child to scene
  scene->addChild(layer);

  // return the scene
  return scene;
}

bool Thunder::init() {
  if (!Layer::init()) {
    return false;
  }

  count = 0;

  isMove = false;  // �Ƿ����ɴ�
  visibleSize = Director::getInstance()->getVisibleSize();


  // ��������
  auto bgsprite = Sprite::create("bg.jpg");
  bgsprite->setPosition(visibleSize / 2);
  bgsprite->setScale(visibleSize.width / bgsprite->getContentSize().width,
                     visibleSize.height / bgsprite->getContentSize().height);
  this->addChild(bgsprite, 0);

  // �����ɴ�
  player = Sprite::create("player.png");
  player->setAnchorPoint(Vec2(0.5, 0.5));
  player->setPosition(visibleSize.width / 2, player->getContentSize().height);
  player->setName("player");
  this->addChild(player, 1);

  // ��ʾ��ʯ���ӵ�����
  enemysNum = Label::createWithTTF("enemys: 0", "fonts/arial.TTF", 20);
  enemysNum->setColor(Color3B(255, 255, 255));
  enemysNum->setPosition(50, 60);
  this->addChild(enemysNum, 3);
  bulletsNum = Label::createWithTTF("bullets: 0", "fonts/arial.TTF", 20);
  bulletsNum->setColor(Color3B(255, 255, 255));
  bulletsNum->setPosition(50, 30);
  this->addChild(bulletsNum, 3);

  addEnemy(5);        // ��ʼ����ʯ
  preloadMusic();     // Ԥ��������
  playBgm();          // ���ű�������
  explosion();        // ������ը֡����

  // ��Ӽ�����
  addTouchListener();
  addKeyboardListener();
  addCustomListener();

  // ������
  schedule(schedule_selector(Thunder::update), 0.04f, kRepeatForever, 0);

  return true;
}

//Ԥ���������ļ�
void Thunder::preloadMusic() {
	auto audio = SimpleAudioEngine::getInstance();

	audio->preloadBackgroundMusic("music/bgm.mp3");
	audio->preloadEffect("music/explore.wav");
	audio->preloadEffect("music/fire.wav");
}

//���ű�������
void Thunder::playBgm() {
  // Todo
   SimpleAudioEngine::getInstance()->playBackgroundMusic("music/bgm.mp3", true);
}

//��ʼ����ʯ
void Thunder::addEnemy(int n) {
  enemys.clear();
  for (unsigned i = 0; i < 3; ++i) {
    char enemyPath[20];
    sprintf(enemyPath, "stone%d.png", 3 - i);
    double width = visibleSize.width / (n + 1.0),
           height = visibleSize.height - (50 * (i + 1));
    for (int j = 0; j < n; ++j) {
      auto enemy = Sprite::create(enemyPath);
      enemy->setAnchorPoint(Vec2(0.5, 0.5));
      enemy->setScale(0.5, 0.5);
      enemy->setPosition(width * (j + 1), height);
      enemys.push_back(enemy);
      addChild(enemy, 1);
    }
  }
}

// ��ʯ�����ƶ��������µ�һ��(�ӷ���)
void Thunder::newEnemy() {
  // Todo
	for (auto it = enemys.begin(); it != enemys.end(); it++) {
		(*it)->setPosition((*it)->getPosition()+Vec2(0, -50));
	}
	int i;
	if (count == 0) i = 0;
	else if (count == 1) i = 1;
	else i = 2;
	char enemyPath[20];
	sprintf(enemyPath, "stone%d.png", i+1);
	double width = visibleSize.width / (5 + 1.0),
	       height = visibleSize.height - 50;
	for (int j = 0; j < 5; ++j) {
		auto enemy = Sprite::create(enemyPath);
		enemy->setAnchorPoint(Vec2(0.5, 0.5));
		enemy->setScale(0.5, 0.5);
		enemy->setPosition(width * (j + 1)-85, height);
		enemys.push_back(enemy);
		addChild(enemy, 1);
	}
	count = (count + 1) % 3;
}

// �ƶ��ɴ�
void Thunder::movePlane(char c) {
  // Todo
	if (c == 'A') {
		if (player->getBoundingBox().getMinX() > 15) {
			auto moveLeft = MoveBy::create(0.2f, Vec2(-10, 0));
			player->runAction(moveLeft);
		}
	}
	else if (c == 'D') {
		if (player->getBoundingBox().getMaxX() < visibleSize.width - 15) {
			auto moveRight = MoveBy::create(0.2f, Vec2(10, 0));
			player->runAction(moveRight);
		}
	}
}

//�����ӵ�
void Thunder::fire() {
  auto bullet = Sprite::create("bullet.png");
  bullet->setAnchorPoint(Vec2(0.5, 0.5));
  bullets.push_back(bullet);
  bullet->setPosition(player->getPosition());
  addChild(bullet, 1);
  SimpleAudioEngine::getInstance()->playEffect("music/fire.wav", false);
  
  // �Ƴ��ɳ���Ļ����ӵ�
  // Todo
  
    Sprite* temp = bullet;
  bullet->runAction(
      Sequence::create(
		  MoveBy::create(1.0f, Vec2(0, visibleSize.height)),
		  CallFunc::create([temp,bullet,this]() {
	  if (bullet->getPosition().y > visibleSize.height) {
		  bullets.remove(bullet);
		  temp->removeFromParentAndCleanup(true);
	  }
      }),
	  nullptr)
  );
  
  //bullet->runAction(MoveBy::create(1.0f, Vec2(0, visibleSize.height)));

}

// �иը����֡
void Thunder::explosion() {
  // Todo
	auto texture = Director::getInstance()->getTextureCache()->addImage("explosion.png");
	explore.reserve(8);
	for (int i = 0; i < 5; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(191*i,0,191,192)));
		explore.pushBack(frame);
	}
	for (int i = 0; i < 2; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(191*i,192,191,192)));
	}
	auto explore_animation = Animation::createWithSpriteFrames(explore, 0.1f);
	AnimationCache::getInstance()->addAnimation(explore_animation, "exploreAnimation");
}

void Thunder::update(float f) {
  // ʵʱ����ҳ������ʯ���ӵ�����(����ɾ��)
  // Ҫ��������ʾ��ȷ(�ӷ���)
  char str[15];
  sprintf(str, "enemys: %d", enemys.size());
  enemysNum->setString(str);
  sprintf(str, "bullets: %d", bullets.size());
  bulletsNum->setString(str);

  // �ɴ��ƶ�
  if (isMove)
    this->movePlane(movekey);

  static int ct = 0;
  static int dir = 4;
  ++ct;
  if (ct == 120)
    ct = 40, dir = -dir;
  else if (ct == 80) {
    dir = -dir;
    newEnemy();  // ��ʯ�����ƶ��������µ�һ��(�ӷ���)
  }
  else if (ct == 20)
    ct = 40, dir = -dir;

  //��ʯ�����ƶ�
  for (Sprite* s : enemys) {
    if (s != NULL) {
      s->setPosition(s->getPosition() + Vec2(dir, 0));
    }
  }

  // �ַ��Զ����¼�
  EventCustom e("meet");
  _eventDispatcher->dispatchEvent(&e);
}

// �Զ�����ײ�¼�
void Thunder::meet(EventCustom * event) {
  // �ж��ӵ��Ƿ������ʯ��ִ�ж�Ӧ����
  // Todo
	Sprite* temp;
	Sprite* temp1;
	bool flag = false;
	for (auto it = bullets.begin(); it != bullets.end(); it++) {
		for (auto it1 = enemys.begin(); it1 != enemys.end(); it1++) {
			if ((*it)->getPosition().getDistance((*it1)->getPosition()) < 25) {
				SimpleAudioEngine::getInstance()->playEffect("music/explore.wav", false);
				auto exploreAnimation = Animate::create(AnimationCache::getInstance()->getAnimation("exploreAnimation"));
				temp = (*it);
				temp1 = (*it1);
				bullets.erase(it);
				(*it1)->runAction(
					Sequence::create(
						exploreAnimation,
						CallFunc::create([temp, temp1, this] {
					enemys.remove(temp1);
 					// temp->removeFromParentAndCleanup(true);
					temp1->removeFromParentAndCleanup(true);
				    }),
						nullptr
					)
				);
				flag = true;
				break;
			}
		}
		if (flag == true) {
			temp->removeFromParentAndCleanup(true);
			break;
		}
	}

  // �ж���Ϸ�Ƿ������ִ�ж�Ӧ����
  // Todo
	auto exploreAnimation = Animate::create(AnimationCache::getInstance()->getAnimation("exploreAnimation"));
	for (auto it = enemys.begin(); it != enemys.end(); it++) {
		if ((*it)->getBoundingBox().getMinY() <= player->getBoundingBox().getMaxY()) {
			Sprite* gameover = Sprite::create("gameOver.png");
			gameover->setPosition(visibleSize.width / 2, visibleSize.height / 2);
			this->addChild(gameover, 2);
			player->runAction(
			  Sequence::create(
			    exploreAnimation,
				  CallFunc::create([this] {
					 _eventDispatcher->removeAllEventListeners();
					 unschedule(schedule_selector(Thunder::update));
			      }  
				),
				nullptr
			  )
			);
		}
	}
}

// ����Զ��������
void Thunder::addCustomListener() {
  // Todo
	auto meetListener = EventListenerCustom::create("meet", CC_CALLBACK_1(Thunder::meet, this));
	_eventDispatcher->addEventListenerWithFixedPriority(meetListener, 1);
}

// ��Ӽ����¼�������
void Thunder::addKeyboardListener() {
  // Todo
	auto KeyBoardListener = EventListenerKeyboard::create();
	KeyBoardListener->onKeyPressed = CC_CALLBACK_2(Thunder::onKeyPressed, this);
	KeyBoardListener->onKeyReleased = CC_CALLBACK_2(Thunder::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(KeyBoardListener, player);
}

void Thunder::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
  switch (code) {
  case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
  case EventKeyboard::KeyCode::KEY_CAPITAL_A:
  case EventKeyboard::KeyCode::KEY_A:
    movekey = 'A';
    isMove = true;
    break;
  case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
  case EventKeyboard::KeyCode::KEY_CAPITAL_D:
  case EventKeyboard::KeyCode::KEY_D:
    movekey = 'D';
    isMove = true;
    break;
  case EventKeyboard::KeyCode::KEY_SPACE:
    fire();
    break;
  }
}

void Thunder::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
  switch (code) {
  case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
  case EventKeyboard::KeyCode::KEY_A:
  case EventKeyboard::KeyCode::KEY_CAPITAL_A:
  case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
  case EventKeyboard::KeyCode::KEY_D:
  case EventKeyboard::KeyCode::KEY_CAPITAL_D:
    isMove = false;
    break;
  }
}

// ��Ӵ����¼�������
void Thunder::addTouchListener() {
  // Todo
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(Thunder::onTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(Thunder::onTouchEnded, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(Thunder::onTouchMoved, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, player);
}

// ����������ڵ�
bool Thunder::onTouchBegan(Touch *touch, Event *event) {
// ����жϱ���д�����������������onTouchMoved�����л�����϶����ٵ����
  if (player->getBoundingBox().containsPoint(touch->getLocation())) {
	  isClick = true;
  }
  else {
	  fire();
  }
  return true;
  // ����true����ִ��onTouchEnded �� onTouchMoved����
}

void Thunder::onTouchEnded(Touch *touch, Event *event) {
  isClick = false;
}

// ����갴ס�ɴ���ɿ��Ʒɴ��ƶ� (�ӷ���)
void Thunder::onTouchMoved(Touch *touch, Event *event) {
  // Todo
  // getLocation ���ص���GL���꣬getLocationInView���ص�����Ļ����
	if (isClick) 
	{
		Point p = touch->getLocation();
		player->setPosition(Vec2(p.x, player->getPosition().y));
	}
	
}
