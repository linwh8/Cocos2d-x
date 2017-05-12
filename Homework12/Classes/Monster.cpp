#include"Monster.h"
USING_NS_CC;

Factory* Factory::factory = NULL;

Factory::Factory() {
	initSpriteFrame();
}

// 获取单例
Factory* Factory::getInstance() {
	if (factory == NULL) {
		factory = new Factory();
	}
	return factory;
}

// 生成monster死亡的动画帧并存入到vector数组中
void Factory::initSpriteFrame() {
	auto texture = Director::getInstance()->getTextureCache()->addImage("Monster.png");
	monsterDead.reserve(4);
	for (int i = 0; i < 4; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(258 - 48 * i, 0, 42, 42)));
		monsterDead.pushBack(frame);
	}
	// 将这个动画加入到AnimationCache中
	auto monsterdead_animation = Animation::createWithSpriteFrames(monsterDead, 0.1f);
	AnimationCache::getInstance()->addAnimation(monsterdead_animation, "monsterDeadAnimation");
}

// 新建一个monster
Sprite* Factory::createMonster() {
	Sprite* mons = Sprite::create("Monster.png", CC_RECT_PIXELS_TO_POINTS(Rect(364,0,42,42)));
	monster.pushBack(mons);
	return mons;
}

// 遍历删除与参数一致的monster
void Factory::removeMonster(Sprite* sp) {
   cocos2d:Vector<Sprite*>::iterator it = monster.begin();
	for (; it != monster.end(); it++) {
		if (*it == sp) {
			monster.erase(it);
			return;
		}
	}
}

// 实现monster朝着player的方向移动
void Factory::moveMonster(Vec2 playerPos,float time){
	cocos2d:Vector<Sprite*>::iterator it = monster.begin();
	for (; it != monster.end(); it++) {
		Vec2 monsterPos = (*it)->getPosition();
		Vec2 direction = playerPos - monsterPos;
		direction.normalize(); // 得到该向量的单位向量，即向量除以向量的模
		(*it)->runAction(MoveBy::create(time, direction*30));
	}
}

// 当monster在rect的范围内时，返回指向该monster的指针
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