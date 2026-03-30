
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <algorithm>


struct Data1 {  // 畢業生數量(可能有重複)
    std::vector<int> orders;
    int graduation_num;
};

struct Node1 { // 一個node可以有複數個畢業生人數
    Data1 data_list[3];
    Node1 *child[4];
    Node1 *parent;
    Node1() { //建構子
        parent = nullptr;
        for (int i = 0; i < 3; i++) child[i] = nullptr;
        for (int i = 0; i < 2; i++) data_list[i].graduation_num = -1;
    }
};

struct Node2 {
    std::vector<int> orders;
    std::string school_name;
    Node2 *left_child = nullptr;
    Node2 *right_child = nullptr;
    int height = 1;
};

struct Data {
    int order;
    std::string school_num;
    std::string school_name;
    std::string major_num;
    std::string major_name;
    std::string time_type;
    std::string level;
    int student_num;
    int teacher_num;
    int graduate_num;
    std::string district;
    std::string type;
};


class TwoThreeTree {
    private:
        Node1* root;
    public:
        void swap(Data1 &d1, Data1 &d2) {
            Data1 temp;
            temp = d1;
            d1 = d2;
            d2 = temp;

        }

    void ClearTree(Node1* &node) {
        if (node == nullptr) return;

        for (int i = 0; i < 3; i++) {
            if (node->child[i]) {
                ClearTree(node->child[i]);
            }
        }

        delete node;
        node = nullptr;
    }

        void Clear() {
            ClearTree(root);
        }

        Node1* GetRoot() {
            return root;
        }

        bool SameNodeInsert(Node1* cur, Data1& newdata) { //加在已有的node上
            for (int i = 0; i < 2; i++) { //可能同值
                if (cur -> data_list[i].graduation_num == newdata.graduation_num) {
                    cur -> data_list[i].orders.push_back(newdata.graduation_num);
                    return true;
                }
            }
            if (cur -> data_list[0].graduation_num == -1) {
                cur -> data_list[0] = newdata;
                return true;
            }
            if (cur -> data_list[1].graduation_num != -1) { //把滿的情況排除
                return false;
            }
            //這裡一定還有空間加 //三種情況


            if (cur -> data_list[0].graduation_num > newdata.graduation_num) { //小於交換
                cur -> data_list[1] = cur -> data_list[0];
                cur -> data_list[0] = newdata;
                return true;
            } else {
                cur -> data_list[1] = newdata;
                return true;
            }
            return true;
        }

        void Split(Node1* cur, Data1 &data, Node1 *left_c=nullptr, Node1 *right_c=nullptr) {
            Data1 min = cur -> data_list[0];
            Data1 max = cur -> data_list[1];
            Data1 mid = data;
            if (mid.graduation_num < min.graduation_num) swap(mid, min);
            if (mid.graduation_num > max.graduation_num) swap(mid, max);

            //三種情況
            Node1* left = new Node1();
            Node1* right = new Node1();

            left -> data_list[0] = min;
            right -> data_list[0] = max;

            Node1* children[4] = {nullptr, nullptr, nullptr, nullptr};
            // 原本的 3 個 child
            Node1* old[3] = {
                cur->child[0],
                cur->child[1],
                cur->child[2]
            };
            if (cur -> child[0] != nullptr) {

                // 把新帶上來的 child 插進去（依 data 決定位置）
                if (data.graduation_num < cur->data_list[0].graduation_num) {
                    children[0] = left_c;
                    children[1] = right_c;
                    children[2] = old[1];
                    children[3] = old[2];
                }
                else if (cur->data_list[1].graduation_num == -1 ||
                        data.graduation_num < cur->data_list[1].graduation_num) {
                    children[0] = old[0];
                    children[1] = left_c;
                    children[2] = right_c;
                    children[3] = old[2];
                }
                else {
                    children[0] = old[0];
                    children[1] = old[1];
                    children[2] = left_c;
                    children[3] = right_c;
                }
            }

            // child 分配
            if (children[0] != nullptr) {
                left->child[0] = children[0];
                left->child[1] = children[1];
                right->child[0] = children[2];
                right->child[1] = children[3];

                // 設 parent
                for (int i = 0; i < 2; i++) {
                    if (left->child[i]) left->child[i]->parent = left;
                    if (right->child[i]) right->child[i]->parent = right;
                }
            }

            //遇到root
            if (cur == root) {
                Node1* newroot = new Node1();
                newroot->data_list[0] = mid;
                newroot -> child[0] = left;
                newroot -> child[1] = right;

                left -> parent = newroot;
                right -> parent = newroot;
                root = newroot;
                delete cur;
                return;
            }
            Node1* p = cur -> parent;
            bool inserted = SameNodeInsert(p, mid);

            if (inserted) {
                // parent 有空位-> 重建 child

                Node1* temp[3];

                // 接到對應位置
                if (p->child[0] == cur) {
                    temp[0] = left;
                    temp[1] = right;
                    temp[2] = p->child[1];
                }
                else {
                    temp[0] = p->child[0];
                    temp[1] = left;
                    temp[2] = right;
                }

                // 直接重建
                for (int i = 0; i < 3; i++) {
                    p->child[i] = temp[i];
                    if (temp[i]) temp[i]->parent = p;
                }

                delete cur;
                return;
            }

            Split(p, mid, left, right);
            delete cur;
        }


        void Insert(const Data &main_list) {
            Data1 data;
            data.graduation_num = main_list.graduate_num;
            data.orders.push_back(main_list.order);

            if (root == nullptr) {
                root = new Node1();
                SameNodeInsert(root, data);
                return;
            }

            Node1* cur = root;
            while (cur != nullptr) {
                // 先檢查當前節點有沒有重複的資料
                for (int i = 0; i < 2; i++) {
                    if (cur->data_list[i].graduation_num == data.graduation_num) {
                        cur->data_list[i].orders.push_back(main_list.order);
                        return; // 直接結束，不需要再向下搜尋或 Split
                    }
                }

                // 如果是葉子節點，這就是要插入的位置
                if (cur->child[0] == nullptr) {
                    if (!SameNodeInsert(cur, data)) {
                        Split(cur, data);
                    }
                    return;
                }

                // 決定下一步往哪走
                if (data.graduation_num < cur->data_list[0].graduation_num) {
                    cur = cur->child[0];
                } else if (cur->data_list[1].graduation_num == -1 ||
                        data.graduation_num < cur->data_list[1].graduation_num) {
                    cur = cur->child[1];
                } else {
                    cur = cur->child[2];
                }
            }
        }
};

class AVLTree {
    private:
        Node2* root;

    public:




};


class Tool {
    public:

        // 計算層數
        static int CaLevel(int index) {
            int level = 0;
            while(index > 0) {
                index /= 2;
                level++;
            }
            return level;
        }

        //刪掉非數字符號
        static std::string StringGeneralization(const std::string &num) {
            std::string output;
            if (num[0] > '9' || num[0] < '0') {

                for (int i = 0; i < num.size(); i++) {
                    if (num[i] <= '9' && num[i] >= '0') {
                        output += num[i];
                    }
                }
                return output;
            }
            return num;
        }

        static bool IsDigit(char w) {
            if (w >= '0' && w <= '9') {
                return true;
            }
            return false;
        }

        static int CalHeight(Node1* root) {
            if (root == nullptr) return 0;
            if (root->child[0] == nullptr) return 1;

            return 1 + CalHeight(root->child[0]);
        }

        static int CalNodes(const Node1* root) {
            if (root == nullptr) return 0;

            // 如果是葉子節點，直接回傳 1 (節點本身)
            // 2-3 樹中，若 child[0] 為空，則該節點必為葉子
            if (root->child[0] == nullptr) return 1;

            int count = 1; // 計入當前節點

            // 遍歷所有可能的子節點 (0, 1, 2)
            for (int i = 0; i < 3; i++) {
                if (root->child[i] != nullptr) {
                    count += CalNodes(root->child[i]);
                }
            }

            return count;
        }

        static void readFile(const std::string& prefix, std::vector<Data>& list) {
            std::ifstream file("input" + prefix + ".txt", std::ios::binary);
            std::string line;
            int order = 0;

            // 讀前3 header
            for (int i = 0; i < 3; ++i) {
                std::getline(file, line);
            }

            std::string school_num;
            std::string level, student_num, teacher_num, graduate_num;

            while (std::getline(file, school_num, '\t')) {
                Data data;
                order++;

                std::getline(file, data.school_name, '\t');
                std::getline(file, data.major_num, '\t');
                std::getline(file, data.major_name, '\t');
                std::getline(file, data.time_type, '\t');
                try {
                    std::getline(file, level, '\t');
                }
                catch (...) {
                    level = "";
                }
                std::getline(file, student_num, '\t');
                std::getline(file, teacher_num, '\t');
                std::getline(file, graduate_num, '\t');
                std::getline(file, data.district, '\t');
                std::getline(file, data.type, '\n');

                data.order = order;
                data.school_num = school_num;
                if (level != "") {
                    data.level = level;
                }
                data.student_num = std::stoi(Tool::StringGeneralization(student_num));
                data.teacher_num = std::stoi(Tool::StringGeneralization(teacher_num));
                data.graduate_num = std::stoi(Tool::StringGeneralization(graduate_num));

                list.push_back(data);
            }

        }

        static void printTask1(TwoThreeTree &tree23, const std::vector<Data> &main_list) {
            std::cout << "Tree height = " << CalHeight(tree23.GetRoot()) << std::endl;
            std::cout << "Number of nodes = " << CalNodes(tree23.GetRoot()) << std::endl;
            int count = 1;
            for (int i = 0; i < 2; i++) {
                if (tree23.GetRoot() -> data_list[i].graduation_num != -1) {
                    for (int j = 0; j < tree23.GetRoot() -> data_list[i].orders.size(); j++) {
                        int order = tree23.GetRoot() -> data_list[i].orders[j];
                        std::cout << count << ": [" <<  order << "] " << main_list[order-1].school_name << ", " <<
                        main_list[order-1].major_name << ", " <<  main_list[order-1].time_type << ", " <<
                        main_list[order-1].level << ", " << main_list[order-1].student_num << ", " <<
                        main_list[order-1].graduate_num << std::endl;
                    }

                }
            }
            std::cout << "\n";


        }

        static void printTask2() {
        }

        static void printTask3() {
        }

};


class Sys {
    private:
        std::vector<Data> main_list;
    public:

        void Clear() {
            main_list.clear();
        }

        void task1() {
            std::string prefix;
            TwoThreeTree tree23;
            while (true) {
                std::cout << "Input a file number ([0] Quit): ";

                std::cin >> prefix;
                if (prefix == "0") {
                    std::cout << '\n';
                    return;
                }
                if (!std::ifstream("input" + prefix + ".txt")) {
                    std::cout << "\n### input" + prefix + ".txt does not exist! ###\n\n";
                    continue;
                } else {
                    break;
                }
            }
            Clear();
            Tool::readFile(prefix, main_list);
            for (int i = 0; i < main_list.size(); i++) {
                tree23.Insert(main_list[i]);
            }
            Tool::printTask1(tree23, main_list);
            tree23.Clear();
        }



        void task2() {
            std::string prefix;
            AVLTree avl_tree;
            std::vector<Data> main_list2;
            while (true) {
                std::cout << "Input a file number ([0] Quit): ";
                std::cin >> prefix;
                if (prefix == "0") {
                    std::cout << '\n';
                    return;
                }
                if (!std::ifstream("input" + prefix + ".txt")) {
                    std::cout << "\n### input" + prefix + ".txt does not exist! ###\n\n";
                    continue;
                } else {
                    break;
                }
            }
        }


        void task3() {
        }

        void task4() {


        }

        void run() {
            std::string task_input;
            int task_number;
            while (true) {
                std::cout << "* Data Structures and Algorithms *\n";
                std::cout << "*** Heap Construction and Use ****\n";
                std::cout << "* 0. QUIT                        *\n";
                std::cout << "* 1. Build 23 tree               *\n";
                std::cout << "* 2. Build AVL tree              *\n";
                std::cout << "**********************************\n";
                std::cout << "Input a choice(0, 1, 2): ";
                std::cin >> task_input;
                try {
                    task_number = std::stoi(task_input);
                } catch (...) {
                    std::cout << "\nCommand does not exist!\n\n";
                    continue;
                }
                if (task_number == 1) {
                    std::cout << "\n";
                    task1();
                } else if (task_number == 2) {
                    std::cout << "\n";
                    task2();
                } else if (task_number == 0) {
                    break;
                } else {
                    std::cout << "\nCommand does not exist!\n\n";
                }
                /*
                 else if (task_number == 3) {
                    std::cout << "\n";
                    task3();
                } else if (task_number == 4) {
                    std::cout << "\n";
                    task4();
                */
            }
        }


};

        int main () {
            Sys system;
            system.run();
            return 0;
        }