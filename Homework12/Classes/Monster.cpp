#include"Monster.h"
USING_NS_CC;

Factory* Factory::factory = NULL;

Factory::Factory() {
	initSpriteFrame();
}

// ��ȡ����
Factory* Factory::getInstance() {
	if (factory == NULL) {
		factory = new Factory();
	}
	return factory;
}

// ����monster�����Ķ���֡�����뵽vector������
void Factory::initSpriteFrame() {
	auto texture = Director::getInstance()->getTextureCache()->addImage("Monster.png");
	monsterDead.reserve(4);
	for (int i = 0; i < 4; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(258 - 48 * i, 0, 42, 42)));
		monsterDead.pushBack(frame);
	}
	// ������������뵽AnimationCache��
	auto monsterdead_animation = Animation::createWithSpriteFrames(monsterDead, 0.1f);
	AnimationCache::getInstance()->addAnimation(monsterdead_animation, "monsterDeadAnimation");
}

// �½�һ��monster
Sprite* Factory::createMonster() {
	Sprite* mons = Sprite::create("Monster.png", CC_RECT_PIXELS_TO_POINTS(Rect(364,0,42,42)));
	monster.pushBack(mons);
	return mons;
}

// ����ɾ�������һ�µ�monster
void Factory::removeMonster(Sprite* sp) {
   cocos2d:Vector<Sprite*>::iterator it = monster.begin();
	for (; it != monster.end(); it++) {
		if (*it == sp) {
			monster.erase(it);
			return;
		}
	}
}

// ʵ��monster����player�ķ����ƶ�
void Factory::moveMonster(Vec2 playerPos,float time){
	cocos2d:Vector<Sprite*>::iterator it = monster.begin();
	for (; it != monster.end(); it++) {
		Vec2 monsterPos = (*it)->getPosition();
		Vec2 direction = playerPos - monsterPos;
		direction.normalize(); // �õ��������ĵ�λ����������������������ģ
		(*it)->runAction(MoveBy::create(time, direction*30));
	}
}

// ��monster��rect�ķ�Χ��ʱ������ָ���monster��ָ��
Sprite* Factory::collider(Rect rect) {
    cocos2d:Vector<Sprite*>::iterator it = monster.begin();
	for (; it != monster.end(); it++) {
		Vec2 monsterPos = (*it)->getPosition();
		if (rect.containsPoint(monsterPos)) {
			return (*it);
		}
	}
	return NULL;
}