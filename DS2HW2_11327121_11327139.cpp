// 11327121 侯至謙 11327139 李士璿

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <limits>


struct Data1 {
    std::vector<int> orders;
    int graduation_num;

};

struct Node1 {
    Data1 data_list[2];
    Node1* child[3];
    Node1* parent;

    Node1() : parent(nullptr) {
        child[0] = child[1] = child[2] = nullptr;
        data_list[0].graduation_num = -1;
        data_list[1].graduation_num = -1;
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
        bool isEmpty(const Data1& d) {
            return d.graduation_num == -1;
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

        void SetRoot() {
            root = nullptr;
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
                // ===== parent 有空位 → 重建 child =====

                Node1* temp[3];

                // 收集 child（把 cur 換成 left/right）
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
            Data1 data{};
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

        void GetData(std::vector<int> &rec, int &num, Node1 *cur) {
                if (cur->data_list[1].graduation_num != -1) {
                    for (int i = 0; i < cur->data_list[1].orders.size()-1; i++) {
                        if (num > 0) {
                            rec.push_back(cur->data_list[1].orders[i]);
                            num--;
                        }
                    }
                } else {
                    for (int i = 0; i < cur->data_list[0].orders.size()-1; i++) {
                        if (num > 0) {
                            rec.push_back(cur->data_list[0].orders[i]);
                            num--;
                        }
                    }
                }
        }

        void GetTopK(Node1* cur, std::vector<int>& rec, int& k) {
            if (cur == nullptr || k <= 0) return;

            // ===== 3-node =====
            if (cur->data_list[1].graduation_num != -1) {

                // 最右（最大區間）
                GetTopK(cur->child[2], rec, k);

                // 最大 key
                if (k > 0) {
                    rec.push_back(cur->data_list[1].graduation_num);
                    k--;
                }

                // 中間
                GetTopK(cur->child[1], rec, k);
            }

            // ===== 共同部分（2-node + 3-node）=====

            // 次大 key（或 2-node 唯一 key）
            if (k > 0) {
                rec.push_back(cur->data_list[0].graduation_num);
                k--;
            }

            // 最左（最小區間）
            GetTopK(cur->child[0], rec, k);
        }


        void Search(std::vector<int> &rec, int &num) {
            int k = num;
            GetTopK(root, rec, k);
        }


};

class AVLTree {
    private:
        Node2* root;

        int getHeight(Node2* node) {
            if (node == nullptr) return 0;
            return node->height;
        }

        int getBalance(Node2* node) {
            if (node == nullptr) return 0;
            return getHeight(node->left_child) - getHeight(node->right_child);
        }

        Node2* rightRotate(Node2* y) {
            Node2* x = y->left_child;
            Node2* T2 = x->right_child;

            // 執行旋轉
            x->right_child = y;
            y->left_child = T2;

            // 更新高度
            y->height = std::max(getHeight(y->left_child), getHeight(y->right_child)) + 1;
            x->height = std::max(getHeight(x->left_child), getHeight(x->right_child)) + 1;

            return x;
        }

        Node2* leftRotate(Node2* x) {
            Node2* y = x->right_child;
            Node2* T2 = y->left_child;

            // 執行旋轉
            y->left_child = x;
            x->right_child = T2;

            // 更新高度
            x->height = std::max(getHeight(x->left_child), getHeight(x->right_child)) + 1;
            y->height = std::max(getHeight(y->left_child), getHeight(y->right_child)) + 1;

            return y;
        }

        Node2* insertNode(Node2* node, const Data& data) {
            // 一般 BST 插入
            if (node == nullptr) {
                Node2* newNode = new Node2();
                newNode->school_name = data.school_name;
                newNode->orders.push_back(data.order);
                return newNode;
            }

            if (data.school_name < node->school_name) {
                node->left_child = insertNode(node->left_child, data);
            } else if (data.school_name > node->school_name) {
                node->right_child = insertNode(node->right_child, data);
            } else {
                // 如果學校名稱相同，將序號加入同一個節點的 orders 中
                node->orders.push_back(data.order);
                return node;
            }

            // 更新當前節點的高度
            node->height = 1 + std::max(getHeight(node->left_child), getHeight(node->right_child));

            // 檢查是否失衡
            int balance = getBalance(node);

            // Left Left Case (LL)
            if (balance > 1 && data.school_name < node->left_child->school_name)
                return rightRotate(node);

            // Right Right Case (RR)
            if (balance < -1 && data.school_name > node->right_child->school_name)
                return leftRotate(node);

            // Left Right Case (LR)
            if (balance > 1 && data.school_name > node->left_child->school_name) {
                node->left_child = leftRotate(node->left_child);
                return rightRotate(node);
            }

            // Right Left Case (RL)
            if (balance < -1 && data.school_name < node->right_child->school_name) {
                node->right_child = rightRotate(node->right_child);
                return leftRotate(node);
            }

            return node; // 回傳未改變的節點指標
        }

        void clearTree(Node2* &node) {
            if (node == nullptr) return;
            clearTree(node->left_child);
            clearTree(node->right_child);
            delete node;
        }

        int countNodes(Node2* node) const {
            if (node == nullptr) return 0;
            return 1 + countNodes(node->left_child) + countNodes(node->right_child);
        }

    public:
        AVLTree() : root(nullptr) {}

        void Insert(const Data& data) {
            root = insertNode(root, data);
        }

        void Clear() {
            clearTree(root);
            root = nullptr;
        }

        Node2* GetRoot() const {
            return root;
        }

        int GetHeight() {
            return getHeight(root);
        }

        int GetTotalNodes() const {
            return countNodes(root);
        }
        Node2* SearchNode2(Node2* node, const std::string& src_schoolname) {

            if (node == nullptr) return nullptr;
            if (node->school_name == src_schoolname) return node;


            if (src_schoolname < node->school_name) {
                return SearchNode2(node->left_child, src_schoolname);
            } else {
                return SearchNode2(node->right_child, src_schoolname);
            }
        }
};


class Tool {
    public:
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
                        count++;
                    }

                }
            }
            std::cout << "\n";
        }

        static void printTask2(AVLTree &avl_tree, const std::vector<Data> &main_list) {
            Node2* root = avl_tree.GetRoot();
            if (root == nullptr) {
                std::cout << "AVL Tree is empty." << std::endl;
                return;
            }

            std::cout << "Tree height = " << avl_tree.GetHeight() << std::endl;
            std::cout << "Number of nodes = " << avl_tree.GetTotalNodes() << std::endl;

            int count = 1;
            // 輸出 Root 節點內所有的資料
            for (size_t i = 0; i < root->orders.size(); i++) {
                int order = root->orders[i];

                const Data& d = main_list[order - 1];

                std::cout << count << ": [" << d.order << "] "
                          << d.school_name << ", "
                          << d.major_name << ", "
                          << d.time_type << ", "
                          << d.level << ", "
                          << d.student_num << ", "
                          << d.graduate_num << std::endl;
                count++;
            }
            std::cout << "\n";
        }

        static void printTask3(std::vector<int> &rec, const std::vector<Data> &main_list) {
            int count = 1;
            for (int i = 0; i < rec.size(); i++) {
                const Data& d = main_list[rec[i] - 1];
                std::cout << count << "[" << rec[i] << "]" << d.school_name << ", "
                                                           << d.major_name << ", "
                                                           << d.time_type << ", "
                                                           << d.level << ", "
                                                           << d.student_num << ", "
                                                           << d.graduate_num << ", "
                                                           << std::endl;

            }
        }
        static void printTask4(Node2* goalNode, int k, const std::vector<Data> &main_list) {
            std::vector<Data> records;


            for (int order : goalNode->orders) {
                records.push_back(main_list[order - 1]);
            }

            std::sort(records.begin(), records.end(), [](const Data& a, const Data& b) {
                if (a.graduate_num != b.graduate_num) {
                    return a.graduate_num > b.graduate_num;
                }
                return a.order < b.order;
            });


            int print_count = 0;
            int last_graduate_num = -1;

            for (size_t i = 0; i < records.size(); i++) {

                if (print_count >= k && records[i].graduate_num != last_graduate_num) {
                    break;
                }

                const Data& d = records[i];
                std::cout << print_count + 1 << ": [" << d.order << "] "
                          << d.school_name << ", "
                          << d.major_name << ", "
                          << d.time_type << ", "
                          << d.level << ", "
                          << d.student_num << ", "
                          << d.graduate_num << std::endl;

                last_graduate_num = d.graduate_num; // 更新上一筆的數值
                print_count++;
            }


            std::cout <<std::endl;
        }

};


class Sys {
    private:
        std::vector<Data> main_list;
        AVLTree avl_tree;
        TwoThreeTree tree23;
    public:

        void Clear() {
            main_list.clear();
        }

        void task1() {
            std::string prefix;
            while (true) {
                std::cout << "\nInput a file number ([0] Quit): ";
                Clear();
                avl_tree.Clear();

                std::cin >> prefix;
                if (prefix == "0") {
                    return;
                }
                if (!std::ifstream("input" + prefix + ".txt")) {
                    std::cout << "\n### input" + prefix + ".txt does not exist! ###\n";
                    continue;
                } else {
                    break;
                }
            }
            Tool::readFile(prefix, main_list);

            for (int i = 0; i < main_list.size(); i++) {
                if (i == 0) tree23.SetRoot();
                tree23.Insert(main_list[i]);
            }
            Tool::printTask1(tree23, main_list);
            tree23.Clear();
        }


        void task2() {
            // 檢查任務一是否已經載入資料
            if (main_list.empty()) {
                std::cout << "### Choose 1 first. ###\n";
                return;
            }

            if (avl_tree.GetRoot() != nullptr) {
                std::cout << "### AVL tree has been built. ###" << std::endl;
                Tool::printTask2(avl_tree, main_list);
                return;
            }

            // 直接使用 Sys 類別中現有的 main_list，依序新增至 AVL 樹
            for (int i = 0; i < main_list.size(); i++) {
                avl_tree.Insert(main_list[i]);
            }

            // 輸出結果
            Tool::printTask2(avl_tree, main_list);

        }


        void task3() {
            // 1. 檢查任務一是否已經載入資料
            // 確保 tree23 內有資料可以搜尋
            if (main_list.empty()) {
                std::cout << "### Choose 1 first. ###\n";
                return;
            }

            std::vector<int> search_orders; // 用於儲存搜尋過程中經過的節點編號或鍵值
            int num;

            // 2. 提示使用者輸入要搜尋的排名 K (或是特定鍵值)
            // 這裡假設 num 代表的是「搜尋第 K 筆資料」或「值為 K 的資料」
            std::cout << "Enter K in [1, " << main_list.size() << "]: ";

            // 3. 處理輸入並檢查是否為合法整數
            if (!(std::cin >> num)) {
                std::cin.clear(); // 清除錯誤狀態
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 捨棄緩衝區內容
                return;
            }

            // 4. 檢查範圍是否在 [1, size] 之間
            if (num < 1 || num > (int)main_list.size()) {
                std::cout << "### Out of range. ###\n";
                return;
            }

            // 5. 執行 2-3 Tree 的搜尋
            // search_orders 會傳遞進去，讓 Search 函式在走訪時 push_back 經過的節點
            tree23.Search(search_orders, num);

            Tool::printTask3(search_orders, main_list);
        }

        void task4() {
            if (main_list.empty()) {
                std::cout << "### Choose 1 first. ###\n";
                return;
            }
            if (avl_tree.GetRoot() == nullptr) {
                std::cout << "### Choose 2 first. ###\n";
                return;
            }

            std::string src_name = "";
            int k;

            std::cout << "Enter a college name to search: ";
            std::cin >> src_name;

            Node2* goal;
            goal = avl_tree.SearchNode2(avl_tree.GetRoot(), src_name);

            if (goal){
                int k;
                std::cout<< "\nEnter K in [1," << goal->orders.size() << "]: ";
                std::cin >> k;
                if (k > goal->orders.size()){
                    return;
                }


            }
            else {
                std::cout << src_name << " is not found!";
            }

        }

        void run() {
            std::string task_input;
            int task_number;
            while (true) {
                std::cout << "\n* Data Structures and Algorithms *\n";
                std::cout << "****** Balanced Search Tree ******\n";
                std::cout << "* 0. QUIT                        *\n";
                std::cout << "* 1. Build 23 tree               *\n";
                std::cout << "* 2. Build AVL tree              *\n";
                std::cout << "* 3. Top-K max search on 23 tree *\n";
                std::cout << "* 4. Exact search on AVL tree    *\n";
                std::cout << "**********************************\n";
                std::cout << "Input a choice(0, 1, 2, 3, 4): ";
                std::cin >> task_input;
                try {
                    task_number = std::stoi(task_input);
                } catch (...) {
                    std::cout << "\nCommand does not exist!\n";
                    continue;
                }
                if (task_number == 1) {
                    task1();
                } else if (task_number == 2) {
                    task2();
                } else if (task_number == 0) {
                    break;
                } else if (task_number == 3) {
                    task3();
                } else if (task_number == 4) {
                    task4();
                } else {
                    std::cout << "\nCommand does not exist!\n";
                }
            }
        }


};

int main () {
    Sys system;
    system.run();
    return 0;
}