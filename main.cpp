#include <bangtal>
#include <string>
#include <cstdio>
#include <ctime>

using namespace bangtal;
using namespace std;

class Door;
typedef shared_ptr<Door> DoorPtr;

class LockedDoor;
typedef shared_ptr<LockedDoor> LockedDoorPtr;

class Flowerpot;
typedef shared_ptr<Flowerpot> FlowerpotPtr;

class Switch;
typedef shared_ptr<Switch> SwitchPtr;

class Item;
typedef shared_ptr<Item> ItemPtr;

class Key;
typedef shared_ptr<Key> KeyPtr;

class Puzzle;
typedef shared_ptr<Puzzle> PuzzlePtr;

class PicturePuzzle;
typedef shared_ptr<PicturePuzzle> PicturePuzzlePtr;

class SpotDifference;
typedef shared_ptr<SpotDifference> SpotDifferencePtr;

// ������ ����
// �ֿ�� �ִ� ��� ������Ʈ�� �������̴�.
class Item : public Object {
public:
	static ItemPtr create(const string& image, ScenePtr scene = nullptr, int x = 0, int y = 0, bool shown = true) {
		auto object = ItemPtr(new Item(image, scene, x, y, shown));
		Object::add(object);
		return object;
	}

protected:
	Item(const string& image, ScenePtr scene, int x, int y, bool shown) : Object(image, scene, x, y, shown) {}

public:
	virtual bool onMouse(int x, int y, MouseAction action) {
		if (!Object::onMouse(x, y, action)) {
			pick();
		}
		return true;
	}
};

// ���� ����
// �������߿� ���� �� �� �ִ� �������� �����.
class Key : public Item {
public:
	static KeyPtr create(const string& image, ScenePtr scene = nullptr, int x = 0, int y = 0, bool shown = true) {
		auto object = KeyPtr(new Key(image, scene, x, y, shown));
		Object::add(object);
		return object;
	}

protected:
	Key(const string& image, ScenePtr scene, int x, int y, bool shown) : Item(image, scene, x, y, shown) {
		this->setScale(0.3f);
	}

public:
	virtual bool onMouse(int x, int y, MouseAction action) {
		if (!Object::onMouse(x, y, action)) {
			showMessage("���踦 �ֿ���.");
			Item::onMouse(x, y, action);
		}
		return true;
	}
};

// �� Ŭ���� ����
// �� �� Ŭ���ϸ� ���� ������ �� �� �� Ŭ���ϸ� ���� ����� �濡 �� �� �ִ�.
class Door : public Object {
public:
	static DoorPtr create(const string& closedImage, const string& openImage, ScenePtr locatedScene = nullptr, ScenePtr connectedScene = nullptr, int x = 0, int y = 0, bool shown = true) {
		auto object = DoorPtr(new Door(closedImage, openImage, locatedScene, connectedScene, x, y, shown));
		Object::add(object);
		return object;
	}

protected:
	Door(const string& closedImage, const string& openImage, ScenePtr locatedScene, ScenePtr connectedScene, int x, int y, bool shown) : Object(closedImage, locatedScene, x, y, shown) {
		_openImage = openImage;
		_closedImage = closedImage;
		_connectedScene = connectedScene;
	}

public:
	void doorOpen() {
		isClosed = false;
		setImage(_openImage);
	}
	virtual bool onMouse(int x, int y, MouseAction action) {
		if (!Object::onMouse(x, y, action)) {
			if (isClosed == true) {
				doorOpen();
			}
			else {
				_connectedScene->enter();
			}
		}
		return true;
	}

protected:
	bool isClosed = true;
	string _openImage, _closedImage;
	ScenePtr _connectedScene;
};

// ��乮 Ŭ���� ����
// ���踦 isHanded�ؾ� �� �� �ִ�.
class LockedDoor : public Door {
public:
	static LockedDoorPtr create(const string& closedImage, const string& openImage, ScenePtr locatedScene = nullptr, ScenePtr connectedScene = nullptr, KeyPtr key = nullptr, int x = 0, int y = 0, bool shown = true, bool locked = true) {
		auto object = LockedDoorPtr(new LockedDoor(closedImage, openImage, locatedScene, connectedScene, key, x, y, shown, locked));
		Object::add(object);
		return object;
	}

protected:
	LockedDoor(const string& closedImage, const string& openImage, ScenePtr locatedScene, ScenePtr connectedScene, KeyPtr key, int x, int y, bool shown, bool locked) : Door(closedImage, openImage, locatedScene, connectedScene, x, y, shown) {
		_key = key;
		isLocked = locked;
	}

public:
	virtual bool onMouse(int x, int y, MouseAction action) {
		if (!Object::onMouse(x, y, action)) {
			if (isLocked == true) {
				if (_key->isHanded()) {
					showMessage("��Ĭ! ���� ���ȴ�.");
					isLocked = false;
				}
				else {
					showMessage("����.");
				}
			}
			else {
				Door::onMouse(x, y, action);
			}
		}
		return true;
	}

protected:
	bool isLocked;
	KeyPtr _key;
};

// ȭ�� Ŭ���� ����
// ȭ���� �� ��ġ�� ���� �� ���谡 ��������.
class Flowerpot : public Object {
public:
	static FlowerpotPtr create(const string& image, ScenePtr scene = nullptr, KeyPtr key = nullptr, int x = 0, int y = 0, bool shown = true) {
		auto object = FlowerpotPtr(new Flowerpot(image, scene, key, x, y, shown));
		Object::add(object);
		return object;
	}

protected:
	Flowerpot(const string& image, ScenePtr scene, KeyPtr key, int x, int y, bool shown) : Object(image, scene, x, y, shown) {
		flowerpot_initial_x = x;
		flowerpot_initial_y = y;
		_scene = scene;
		_key = key;
	}

public:
	virtual bool onMouse(int x, int y, MouseAction action) {
		if (!Object::onMouse(x, y, action)) {
			// ȭ���� �ùٸ� ���� ��ġ���� ��, ���̻� �������� �ʵ��� �ϱ�
			if (flowerpot_Onlocation)
				return true;

			// ȭ���� �巡���Ҷ� �����¿�� �����̵��� �ϱ�
			if (action == MouseAction::MOUSE_DRAG_LEFT) {
				if (flowerpot_initial_x <= 50)
					return true;
				flowerpot_initial_x -= 100;
			}
			else if (action == MouseAction::MOUSE_DRAG_RIGHT) {
				if (flowerpot_initial_x >= 1150)
					return true;
				flowerpot_initial_x += 100;
			}
			else if (action == MouseAction::MOUSE_DRAG_DOWN) {
				if (flowerpot_initial_y <= 50)
					return true;
				flowerpot_initial_y -= 100;
			}
			else if (action == MouseAction::MOUSE_DRAG_UP) {
				if (flowerpot_initial_y >= 150)
					return true;
				flowerpot_initial_y += 100;
			}
			this->locate(_scene, flowerpot_initial_x, flowerpot_initial_y);

			// ȭ���� �ùٸ� ���� �Ű��� �� ������ �� ����
			if (flowerpot_initial_x == flowerpot_Onlocation_x && flowerpot_initial_y == flowerpot_Onlocation_y) {
				flowerpot_Onlocation = true;
				showMessage("���׶�(���� �������� �Ҹ��� ��ȴ�)");
				_key->show();
			}
		}
		return true;
	}

private:
	int flowerpot_initial_x;			// ȭ���� �ʱ� ��ġ
	int flowerpot_initial_y;
	int flowerpot_Onlocation = false;	// ȭ���� �ùٸ� ���� �ִ����� �����ϴ� �÷���
	int flowerpot_Onlocation_x = 1050;	// ȭ���� �ùٸ� ��ġ�� ��ǥ
	int flowerpot_Onlocation_y = 50;
	ScenePtr _scene;
	KeyPtr _key;
};

// ����ġ ����
// ���� ���� ��Ʈ�� ���´�.
class Switch : public Object {
public:
	static SwitchPtr create(const string& image, ScenePtr scene = nullptr, int x = 0, int y = 0, bool shown = true) {
		auto object = SwitchPtr(new Switch(image, scene, x, y, shown));
		Object::add(object);
		return object;
	}

protected:
	Switch(const string& image, ScenePtr scene, int x, int y, bool shown) : Object(image, scene, x, y, shown) {
		_scene = scene;
		this->setScale(3.0f);
		flowerpot_hint = Object::create("images/ȭ��-��Ʈ.png", scene, 1120, 50, false);
	}

public:
	virtual bool onMouse(int x, int y, MouseAction action) {
		if (!Object::onMouse(x, y, action)) {
			if (lighted) {
				_scene->setLight(0.2f);
				flowerpot_hint->show();
				lighted = false;
			}
			else {
				_scene->setLight(1.0f);
				flowerpot_hint->hide();
				lighted = true;
			}
		}
		return true;
	}

private:
	ScenePtr _scene;
	bool lighted = true;
	ObjectPtr flowerpot_hint;
};

// ���� ����
// ���� ���� �Ǵ� Ʋ���׸�ã�Ⱑ ���� Ŭ�����̴�.
// ���� Ŭ������ Ŭ���� �ٸ� ������ ��ȯ�Ǹ�(�̶� ��ȭâ �κ�â ������) ���� ������ ���� �ڷΰ��� ��ư�� ���������� �ִ�.
class Puzzle : public Object {
	static PuzzlePtr create(const string& image, ScenePtr scene = nullptr, ScenePtr connectedScene = nullptr, int x = 0, int y = 0, bool shown = true) {
		auto object = PuzzlePtr(new Puzzle(image, scene, connectedScene, x, y, shown));
		Object::add(object);
		return object;
	}

protected:
	Puzzle(const string& image, ScenePtr scene, ScenePtr connectedScene, int x, int y, bool shown) : Object(image, scene, x, y, shown) {
		_connectedScene = connectedScene;
		prev_scene = scene;

		// �ڷΰ��� ��ư �����
		auto backward_button = Object::create("images/back.png", _connectedScene, 220, 670, true);
		backward_button->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
			setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, true);
			setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, true);
			prev_scene->enter();
			return true;
		});
	}

public:
	virtual bool onMouse(int x, int y, MouseAction action) {
		if (!Object::onMouse(x, y, action)) {
			setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
			setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
			_connectedScene->enter();
		}
		return true;
	}

protected:
	ScenePtr _connectedScene;
	ScenePtr prev_scene;
};


// ���� ���� ����
// ���� ������ Ǯ�� ������ ������ �����.
class PicturePuzzle : public Puzzle {
public:
	static PicturePuzzlePtr create(const string& image, ScenePtr scene = nullptr, ScenePtr connectedScene = nullptr, PuzzlePtr secretPuz = nullptr, int x = 0, int y = 0, bool shown = true) {
		auto object = PicturePuzzlePtr(new PicturePuzzle(image, scene, connectedScene, secretPuz, x, y, shown));
		Object::add(object);
		return object;
	}

protected:
	PicturePuzzle(const string& image, ScenePtr scene, ScenePtr connectedScene, PuzzlePtr secretPuz, int x, int y, bool shown) : Puzzle(image, scene, connectedScene, x, y, shown) {
		_secretPuz = secretPuz;
		init_puzzles();
		//shuffle();
	}

public:
	virtual bool onMouse(int x, int y, MouseAction action) {
		if (!Object::onMouse(x, y, action)) {
			Puzzle::onMouse(x, y, action);
		}
		return true;
	}

private:
	PuzzlePtr _secretPuz;

	static const int num_of_puzzles = 9;	// ���� �� ����
	const int puzzle_size = 200;			// �ȼ�
	const int invisible_puzzle_num = 8;		// �Ⱥ��� ���� ����
	const int x_0 = 374, y_0 = 465;			// ���� ��ǥ
	const int x_1 = x_0 + puzzle_size, y_1 = y_0 - puzzle_size;
	const int x_2 = x_1 + puzzle_size, y_2 = y_1 - puzzle_size;

	struct Position {
		int x, y;
	};

	// ���� �� ��ǥ�� �����ϱ�
	Position position[num_of_puzzles] = {
		{ x_0, y_0 },
		{ x_1, y_0 },
		{ x_2, y_0 },
		{ x_0, y_1 },
		{ x_1, y_1 },
		{ x_2, y_1 },
		{ x_0, y_2 },
		{ x_1, y_2 },
		{ x_2, y_2 }
	};

	// ���� �ϳ��� ����ü
	struct Puzz {
		Position pos;
		ObjectPtr puzObj;
	};

	// ���� ���
	Puzz puzzles[num_of_puzzles];

	// ������ ��ĭ�� �پ��ִ����� �����ϴ� �Լ�
	bool isAdjacent(Position puz_pos, Position invisible_pos) {
		// ��ĭ�� �پ� �ִٰ� �˷��ִ� �÷���
		bool adj_flag = false;

		// Ŭ���� ���� �����ʿ� ��ĭ�� ������
		if (puz_pos.x + puzzle_size == invisible_pos.x && puz_pos.y == invisible_pos.y) {
			adj_flag = true;
		}
		// Ŭ���� ���� ���ʿ� ��ĭ�� ������
		else if (puz_pos.x - puzzle_size == invisible_pos.x && puz_pos.y == invisible_pos.y) {
			adj_flag = true;
		}
		// Ŭ���� ���� ���� ��ĭ�� ������
		else if (puz_pos.y + puzzle_size == invisible_pos.y && puz_pos.x == invisible_pos.x) {
			adj_flag = true;
		}
		// Ŭ���� ���� �Ʒ��� ��ĭ�� ������
		else if (puz_pos.y - puzzle_size == invisible_pos.y && puz_pos.x == invisible_pos.x) {
			adj_flag = true;
		}

		return adj_flag;
	}

	// ������ ��ġ�� ȭ��󿡼� �ٲٴ� �Լ�
	void puzzle_swap(Puzz puzzle, Puzz invisible)
	{
		puzzle.puzObj->locate(_connectedScene, puzzle.pos.x, puzzle.pos.y);
		invisible.puzObj->locate(_connectedScene, invisible.pos.x, invisible.pos.y);
	}

	// ������ ��ǥ�� �ٲٴ� �Լ�
	void position_swap(Puzz& puzzle, Puzz& invisible) {
		int tx = puzzle.pos.x;
		int ty = puzzle.pos.y;

		puzzle.pos.x = invisible.pos.x;
		puzzle.pos.y = invisible.pos.y;

		invisible.pos.x = tx;
		invisible.pos.y = ty;
	}

	// ��� ������� ���ڸ��� �ִ��� Ȯ��
	bool check_alright()
	{
		for (int i = 0; i < num_of_puzzles; i++) {
			if (!(puzzles[i].pos.x == position[i].x && puzzles[i].pos.y == position[i].y)) {
				return false;
			}
		}
		return true;
	}

	void init_puzzles() {
		// ������ ��ǥ�� �������� ��ü�� ����
		for (int i = 0; i < num_of_puzzles; i++) {
			bool visi = true;
			if (i == invisible_puzzle_num)
				visi = false;

			string path = "images/puzzles/" + to_string(i) + ".jpg";
			puzzles[i].puzObj = Object::create(path, _connectedScene, position[i].x, position[i].y, visi);
			puzzles[i].pos.x = position[i].x;
			puzzles[i].pos.y = position[i].y;

			// �� ������ �ݹ� ����
			puzzles[i].puzObj->setOnMouseCallback([&, i](ObjectPtr object, int x, int y, MouseAction action)->bool {
				if (object == puzzles[i].puzObj)
					if (isAdjacent(puzzles[i].pos, puzzles[invisible_puzzle_num].pos)) {
						position_swap(puzzles[i], puzzles[invisible_puzzle_num]);
						puzzle_swap(puzzles[i], puzzles[invisible_puzzle_num]);
					}

				if (check_alright()) {
					showMessage("������ �ϼ����� ����ӿ��� ���𰡰� Ƣ��Դ�.");
					_secretPuz->show();
					for (int i = 0; i < num_of_puzzles; i++)
						puzzles[i].puzObj->setOnMouseCallback(nullptr);
				}
				return true;
			});
		}
	}

	// ���� ����
	void shuffle() {
		srand((unsigned int)time(NULL));
		int random;

		for (int i = 0; i < num_of_puzzles; i++) {
			random = rand() % num_of_puzzles;
			position_swap(puzzles[i], puzzles[random]);
			puzzle_swap(puzzles[i], puzzles[random]);
		}
	}
};

// Ʋ���׸� ã�� ���� ����
// Ʋ���׸��� Ǯ�� ���� ������ ���� ���´�.
class SpotDifference : public Puzzle {
public:
	static SpotDifferencePtr create(const string& image, ScenePtr scene = nullptr, ScenePtr connectedScene = nullptr, DoorPtr secretDoor = nullptr, int x = 0, int y = 0, bool shown = true) {
		auto object = SpotDifferencePtr(new SpotDifference(image, scene, connectedScene, secretDoor, x, y, shown));
		Object::add(object);
		return object;
	}

protected:
	SpotDifference(const string& image, ScenePtr scene, ScenePtr connectedScene, DoorPtr secretDoor, int x, int y, bool shown) : Puzzle(image, scene, connectedScene, x, y, shown) {
		_secretDoor = secretDoor;
		initGame();
	}

public:
	virtual bool onMouse(int x, int y, MouseAction action) {
		if (!Object::onMouse(x, y, action)) {
			Puzzle::onMouse(x, y, action);
		}
		return true;
	}

private:
	DoorPtr _secretDoor;
	ObjectPtr problem;

	class Rect {
	public:
		int centerX, centerY, radius;

		Rect(int cx, int cy, int r) : centerX(cx), centerY(cy), radius(r) {};

		bool checkIn(int x, int y) {
			return (x > centerX - radius) && (x < centerX + radius) && (y > centerY - radius) && (y < centerY + radius);
		}
	};

	static const int num_of_difference = 7;
	const int check_size = 25;
	int count = 0;

	Rect left_rects[num_of_difference] = {
		{ 568, 594, 54 },
		{ 99, 551, 17 },
		{ 383, 482, 16 },
		{ 401, 158, 27 },
		{ 61, 255, 36 },
		{ 592, 421, 35 },
		{ 320, 117, 13 },
	};
	Rect right_rects[num_of_difference] = {
		{ 1186, 594, 54 },
		{ 717, 551, 17 },
		{ 1001, 482, 16 },
		{ 1019, 158, 27 },
		{ 679, 255, 36 },
		{ 1210, 421, 35 },
		{ 938, 117, 13 },
	};

	ObjectPtr left_checks[num_of_difference];
	ObjectPtr right_checks[num_of_difference];

	void initGame() {
		problem = Object::create("images/spot/problem.png", _connectedScene);

		for (int i = 0; i < num_of_difference; i++) {
			left_checks[i] = Object::create("images/spot/check.png", _connectedScene, left_rects[i].centerX - check_size, left_rects[i].centerY - check_size, false);
			right_checks[i] = Object::create("images/spot/check.png", _connectedScene, right_rects[i].centerX - check_size, right_rects[i].centerY - check_size, false);
		}

		problem->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
			for (int i = 0; i < num_of_difference; i++) {
				if (left_rects[i].checkIn(x, y) || right_rects[i].checkIn(x, y)) {
					left_checks[i]->show();
					right_checks[i]->show();
					count++;
				}
			}

			if (count == num_of_difference) {
				showMessage("(������ Ǯ���� ������ �̻��� �Ҹ��� ����)");
				problem->hide();
				_secretDoor->doorOpen();
				_secretDoor->show();
			}
			return true;
		});
	}
};

ScenePtr initGame()
{
	// �� �ʱ�ȭ
	auto start_room = Scene::create("���� ��", "images/���-1.png");
	auto right_room = Scene::create("������ ��", "images/���-2.png");
	auto left_room = Scene::create("���� ��", "images/���-3.png");
	auto puzzle_scene = Scene::create("���� ����", "images/kakao_cut.jpg");
	auto spot_scene = Scene::create("Ʋ�� �׸� ã��", "images/spot/problem.png");
	auto escape = Scene::create("Ż��", "images/Ż��.jpg");

	// ����
	auto left_room_key = Key::create("images/����.png", left_room, 1000, 100, false);

	// ���� �� ��ü �ʱ�ȭ
	auto start_room_rightDoor = LockedDoor::create("images/��-������-����.png", "images/��-������-����.png", start_room, right_room, left_room_key, 800, 270, true, true);
	auto start_room_leftDoor = Door::create("images/��-����-����.png", "images/��-����-����.png", start_room, left_room, 100, 260, true);
	auto flowerpot = Flowerpot::create("images/ȭ��.png", start_room, left_room_key, 550, 150, true);

	// ���� �� ��ü �ʱ�ȭ
	auto left_room_rightDoor = Door::create("images/��-������-����.png", "images/��-������-����.png", left_room, start_room, 900, 250, true);
	left_room_rightDoor->doorOpen();
	auto left_room_switch = Switch::create("images/����ġ.png", left_room, 200, 440, true);

	auto left_room_leftDoor = Door::create("images/��-����-����.png", "images/��-����-����.png", left_room, escape, 260, 290, false);
	left_room_leftDoor->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
		setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
		setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
		showMessage("����� Ż���ϼ̽��ϴ�.");
		return false;
	});

	auto spot_mini = SpotDifference::create("images/Ʋ������.png", left_room, spot_scene, left_room_leftDoor, 100, 420, false);


	// ������ �� ��ü �ʱ�ȭ
	auto right_room_leftDoor = Door::create("images/��-����-����.png", "images/��-����-����.png", right_room, start_room, 320, 270, true);
	right_room_leftDoor->doorOpen();
	auto puzzle_mini = PicturePuzzle::create("images/��������.png", right_room, puzzle_scene, spot_mini, 800, 400, true);

	return start_room;
}

int main(void)
{
	startGame(initGame());
	return 0;
}