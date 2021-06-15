#include<iostream>
#include<deque>
using namespace std;
#define degree  5

struct Node {
	int* index = new int[degree];
	int num=0;
	int type;
	Node* left = NULL, * right = NULL, * father = NULL;
};
struct IndexNode:public Node {
	Node** childs = new Node * [degree + 1];
};
struct DataNode:public Node {
	//string* data = new string[degree];
	//数据假设为string;
	//Node* next=nullptr;
};


Node* root = NULL;

void BPlusCreate();
void BPlusAdd(int data);
void AddDataNode(DataNode* current, int data);
void AddIndexNode(IndexNode* current, Node* child, int data);
void BPlusDelete(int data);
void DeleteDataNode(DataNode* current, int data);
void DeleteIndexNode(IndexNode* current, Node* child);
void BPlusFind(int data);
void BPlusChange(int data);
void BPlusPrint();

//5 8 10 15 16 17 18 6 9 19 20 21 22 7 0
void main() {
	BPlusCreate();
	while (1) {
		int d;
		cout << "删除:";
		cin >> d;
		BPlusDelete(d);
		BPlusPrint();
	}
}

void BPlusCreate() {
	int d;
	cout << "构造B+树(0做结尾):" << endl;
	while (cin>>d) {
		if (d==0)break;
		BPlusAdd(d);
	}
	BPlusPrint();
}

void BPlusAdd(int data) {
	if (root == NULL) {				//当没有任何节点时，创建一个数据节点，然后添加数据
		root = new DataNode();
		root->type = 2;
		AddDataNode((DataNode *)root, data);
	}
	else if (root->type==2) {		//当只有数据节点时，向该节点添加数据
		AddDataNode((DataNode*)root, data);
	}		
	else {									//当根节点是索引节点时，循环递归查找到将要添加数据的数据节点，然后添加数据
		Node* t = root;
		while (t->type!=2) {
			for (int i = 0; i <= t->num; i++) {
				if (i==t->num||data < t->index[i]) {
					IndexNode* p = (IndexNode*)t;
					t = p->childs[i];
					break;
				}
			}
		}
		AddDataNode((DataNode*)t, data);
	}
}

void AddDataNode(DataNode *current,int data) {
	//先将要添加的数据放在节点最后，然后对节点当前数据进行一次冒泡排序，将最后的数据放在合适的位置
	current->index[current->num] = data;			
	for (int i = current->num; i > 0 && current->index[i] < current->index[i - 1]; i--) {
		swap(current->index[i], current->index[i - 1]);
	}
	current->num++;
	//如果当前节点数据个数等于B+树的阶数，进行分裂
	if (current->num == degree) {
		//新建一个右节点，将分裂出去的数据放在右节点，其余数据放在原节点不动
		DataNode *right = new DataNode();		
		right->type = 2;
		current->num = degree / 2;
		for (int i = degree / 2; i < degree; i++) {
			right->index[right->num++] = current->index[i];
		}
		right->right = current->right;
		right->left = current;
		if (current->right)current->right->left = right;
		current->right = right;
		//当没有父节点，及B+树中只有一个数据节点
		if (current->father == NULL) {
			IndexNode* father = new IndexNode();
			father->type = 1;
			root = father;		//根节点变成新生成的父节点
			current->father = father;
			father->childs[0] = current;
			AddIndexNode(father, current, current->right->index[0]);
		}
		else {	//当前节点有父节点
			AddIndexNode((IndexNode*)(current->father), current, current->right->index[0]);
		}
	}
}

void AddIndexNode(IndexNode * current, Node* child,int data) {
	//令新生成的右节点的父节点指向当前节点，并把该节点和索引序号放在当前节点最后，并使用一次冒泡排序，将其放在合适位置
	child->right->father = current;
	current->index[current->num] = data;
	current->childs[current->num + 1] = child->right;
	for (int i = current->num; i > 0 && current->index[i] < current->index[i - 1]; i--) {
		swap(current->index[i], current->index[i - 1]);
		swap(current->childs[i+1], current->childs[i]);
	}
	current->num++;
	//如果当前节点数据个数等于B+树的阶数，进行分裂
	if (current->num == degree) {
		//新建一个右节点，将分裂出去的数据放在右节点，其余数据放在原节点不动,另外需要更改分裂出去的索引节点下儿子节点的父对象
		IndexNode *right = new IndexNode();
		right->type = 1;
		current->num = (degree - 1) / 2;
		for (int i = (degree-1) / 2+1; i < degree; i++) {
			right->childs[right->num] = current->childs[i];
			right->childs[right->num]->father = right;
			right->index[right->num++] = current->index[i];
		}
		right->childs[right->num] = current->childs[degree];
		right->childs[right->num]->father = right;
		right->right = current->right;
		right->left = current;
		if (current->right)current->right->left = right;
		current->right = right;
		//当没有父节点
		if (current->father == NULL) {
			IndexNode* father = new IndexNode();
			father->type = 1;
			root = father;		//改变根节点为新建父节点
			current->father = father;
			father->childs[0] = current;
			AddIndexNode(father, current, current->index[current->num]);
		}
		else {		//有父节点
			AddIndexNode((IndexNode*)(current->father), current, current->index[current->num]);
		}
	}
}

void BPlusDelete(int data) {
	//循环递归查找到将要删除数据的数据节点，然后删除数据
	Node* t = root;
	while (t->type != 2) {
		for (int i = 0; i <= t->num; i++) {
			if (i == t->num || data < t->index[i]) {
				IndexNode* p = (IndexNode*)t;
				t = p->childs[i];
				break;
			}
		}
	}
	DeleteDataNode((DataNode*)t, data);
}

void DeleteDataNode(DataNode* current, int data) {
	//对当前节点进行遍历查找将要删除的数据的，如果找到了，后续数据依次进一；如果没有找到，打印不存在然后退出
	int isExist=false;
	for (int i = 0; i < current->num; i++) {
		if (data == current->index[i]) {
			for (int j = i; j < current->num-1; j++) {
				current->index[j] = current->index[j + 1];
			}
			current->num--;
			isExist = true;
			break;
		}
	}
	if (isExist) {	//如果删除成功，根据当前节点中数据个数来决定是否与兄弟节点合并
		cout << "删除成功" << endl;
		if (current->num < (degree - 1) / 2) {
			if (current->left!=NULL&& current->left->num > (degree - 1) / 2) {	//选择左兄弟节点来填补
				//将当前节点中的数据依次退一，留出空位，然后将左兄弟节点的最后数据放到当前节点第一
				for (int i = current->num; i > 0; i--)current->index[i] = current->index[i - 1];
				current->index[0] = current->left->index[current->left->num-1];
				current->num++;
				current->left->num--;
				//修改父节点。找到当前节点和左节点共同的父节点，根据左节点在父节点的儿子节点里的位置确定要修改数据的位置
				Node* left = current->left, *right = current;
				while (left->father != right->father) {
					left = left->father;
					right = right->father;
				}
				IndexNode* father = (IndexNode*)(left->father);
				for (int i = 0; i < father->num; i++) {
					if (father->childs[i] == left) {
						father->index[i] = current->index[0];
						break;
					}
				}
			}
			else if (current->right != NULL && current->right->num > (degree - 1) / 2) { //选择右兄弟节点来填补
				//将右兄弟节点的第一个数据放到当前节点最后，并将右兄弟节点中的数据依次进一，填补空位
				current->index[current->num] = current->right->index[0];
				current->num++;
				current->right->num--;
				for (int i = 0; i < current->right->num; i++)current->right->index[i] = current->right->index[i + 1];
				//修改父节点。找到当前节点和右节点共同的父节点，根据左节点在父节点的儿子节点里的位置确定要修改数据的位置
				Node* left = current, * right = current->right;
				while (left->father != right->father) {
					left = left->father;
					right = right->father;
				}
				IndexNode* father = (IndexNode*)(left->father);
				for (int i = 0; i < father->num; i++) {
					if (father->childs[i] == left) {
						father->index[i] = current->right->index[0];
						break;
					}
				}
			}
			else {	//进行合并
				if (current->right == NULL && current->left == NULL) { return; }	//如果当前节点的左右节点都是空，直接退出
				else if(current->left==NULL){																//如果左节点为空，选择与右节点合并
					for (int i = 0; i < current->right->num; i++)current->index[current->num++] = current->right->index[i];
					if (current->right->right)current->right->right->left = current;	//注意修改这两个节点之间连接关系
					current->right = current->right->right;
					DeleteIndexNode((IndexNode*)(current->father), current);
				}
				else if(current->right==NULL){															//如果右节点为空，选择与左节点合并
					for (int i = 0; i < current->num; i++)current->left->index[current->left->num++] = current->index[i];
					if (current->right)current->right->left = current->left;					//注意修改这两个节点之间连接关系
					current->left->right = current->right;
					DeleteIndexNode((IndexNode*)(current->father), current->left);
				}
				else {																									//左右节点都存在，选择与两个节点的父节点相同的(左)右节点进行合并，便于合并
					if (current->father == current->right->father) {							//右节点的父节点与当前节点的父节点相同。
						for (int i = 0; i < current->right->num; i++)current->index[current->num++] = current->right->index[i];
						if (current->right->right)current->right->right->left = current;	//注意修改这两个节点之间连接关系
						current->right = current->right->right;
						DeleteIndexNode((IndexNode*)(current->father), current);
					}else if(current->father == current->left->father) {						//左节点的父节点与当前节点的父节点相同。
						for (int i = 0; i < current->num; i++)current->left->index[current->left->num++] = current->index[i];
						if (current->right)current->right->left = current->left;				//注意修改这两个节点之间连接关系
						current->left->right = current->right;
						DeleteIndexNode((IndexNode*)(current->father), current->left);
					}
				}
			}
		}
	}
	else {
		cout << "不存在" << endl;
		return;
	}
}

void DeleteIndexNode(IndexNode* current, Node* child) {
	//对当前节点进行遍历查找将要删除的数据的（根据child节点在当前节点的儿子节点的位置），找到后，后续数据依次进一
	for (int i = 0; i < current->num; i++) {
		if (child == current->childs[i]) {
			delete current->childs[i + 1];
			for (int j = i; j < current->num - 1; j++) {
				current->index[j] = current->index[j + 1];
				current->childs[j + 1] = current->childs[j + 2];
			}
			current->num--;
			break;
		}
	}
	//根据当前节点中数据个数来决定是否与兄弟节点合并
	if (current->num < (degree - 1) / 2) {
		if (current->left != NULL && current->left->num > (degree - 1) / 2) {	//选择左兄弟节点来填补
			for (int i = current->num; i > 0; i--) {
				current->index[i] = current->index[i - 1];
				current->childs[i+1] = current->childs[i];			//注意给左节点的儿子节点留出一个空位
			}
			current->childs[1] = current->childs[0];					
			current->num++;
			//修改父节点。找到当前节点和左节点共同的父节点，根据左节点在父节点的儿子节点里的位置确定要修改数据的位置
			Node* left = current->left, * right = current;
			while (left->father != right->father) {
				left = left->father;
				right = right->father;
			}
			IndexNode* father = (IndexNode*)(left->father);
			for (int i = 0; i < father->num; i++) {
				if (father->childs[i] == left) {
					current->index[0] = father->index[i];		//将父节点相应数据放在当前节点首位
					current->childs[0] = ((IndexNode*)(current->left))->childs[current->left->num];	//左节点最后儿子节点放到当前节点首位
					father->index[i] = current->left->index[--current->left->num];		////左节点最后数据移到父节点
					break;
				}
			}
		}
		else if (current->right != NULL && current->right->num > (degree - 1) / 2) { //选择右兄弟节点来填补
			int temp = current->right->index[0];
			Node* tempnode = ((IndexNode*)(current->right))->childs[0];
			for (int i = 0; i < current->right->num-1; i++) {
				current->right->index[i] = current->right->index[i + 1];
				((IndexNode*)(current->right))->childs[i] = ((IndexNode*)(current->right))->childs[i + 1];			//注意右节点的首个儿子节点也要移到当前节点
			}
			((IndexNode*)(current->right))->childs[current->right->num-1] = ((IndexNode*)(current->right))->childs[current->right->num];
			current->right->num--;
			//修改父节点。找到当前节点和右节点共同的父节点，根据左节点在父节点的儿子节点里的位置确定要修改数据的位置
			Node* left = current, * right = current->right;
			while (left->father != right->father) {
				left = left->father;
				right = right->father;
			}
			IndexNode* father = (IndexNode*)(left->father);
			for (int i = 0; i < father->num; i++) {
				if (father->childs[i] == left) {
					current->childs[current->num] = tempnode;		//当前节点的儿子节点的尾部添加右节点儿子节点的首个
					current->index[current->num++] = father->index[i];	//将父节点相应数据放在当前节点尾部
					father->index[i] = temp;											//右节点首位数据移到父节点
					break;
				}
			}
		}
		else {
			IndexNode* father = (IndexNode*)(current->father);
			int i = 0;
			if (current->right == NULL && current->left == NULL) {		//如果当前节点的左右节点都是空，且当前节点无数据，选择它唯一的儿子节点做根节点
				if (current->num == 0) { root = current->childs[0]; delete current->father; }
			}
			else if (current->left == NULL) {		//如果左节点为空，选择与右节点合并
				//找到父节点相应的数据，将其移到当前节点的尾部
				for (; i < father->num; i++)
					if (father->childs[i] == current) break;
				current->index[current->num++] = father->index[i];		
				//与右节点合并，注意合并右节点的儿子节点并且修改右节点儿子节点的父节点
				int i = 0;
				for (; i < current->right->num; i++) {									
					current->childs[current->num] = ((IndexNode*)(current->right))->childs[i];
					current->childs[current->num]->father = current;
					current->index[current->num++] = current->right->index[i];
				}
				current->childs[current->num] = ((IndexNode*)(current->right))->childs[i];
				current->childs[current->num]->father = current;
				//注意修改这两个节点之间连接关系
				if (current->right->right)current->right->right->left = current;	
				current->right = current->right->right;
				DeleteIndexNode((IndexNode*)(current->father), current);
			}
			else if (current->right == NULL) {		//如果右节点为空，选择与左节点合并
				//找到父节点相应的数据，将其移到左节点的尾部
				for (; i < father->num; i++)
					if (father->childs[i] == current->left) break;
				current->left->index[current->left->num++] = father->index[i];
				//与左节点合并，注意合并当前节点的儿子节点并且当前右节点儿子节点的父节点
				int i = 0;
				for (; i < current->num; i++) {
					((IndexNode*)(current->left))->childs[current->left->num] = current->childs[i];
					((IndexNode*)(current->left))->childs[current->left->num]->father = current->left;
					current->left->index[current->left->num++] = current->index[i];
				}
				((IndexNode*)(current->left))->childs[current->left->num] = current->childs[i];
				((IndexNode*)(current->left))->childs[current->left->num]->father = current->left;
				//注意修改这两个节点之间连接关系
				if (current->right)current->right->left = current->left;
				current->left->right = current->right;
				DeleteIndexNode((IndexNode*)(current->father), current->left);
			}
			else {		//左右节点都存在，选择与两个节点的父节点相同的(左)右节点进行合并，便于合并
				if (current->father == current->right->father) {	//右节点的父节点与当前节点的父节点相同。
					//找到父节点相应的数据，将其移到当前节点的尾部
					for (; i < father->num; i++)
						if (father->childs[i] == current) break;
					current->index[current->num++] = father->index[i];
					//与右节点合并，注意合并右节点的儿子节点并且修改右节点儿子节点的父节点
					int i = 0;
					for (; i < current->right->num; i++) {
						current->childs[current->num] = ((IndexNode*)(current->right))->childs[i];
						current->childs[current->num]->father = current;
						current->index[current->num++] = current->right->index[i];
					}
					current->childs[current->num] = ((IndexNode*)(current->right))->childs[i];
					current->childs[current->num]->father = current;
					//注意修改这两个节点之间连接关系
					if (current->right->right)current->right->right->left = current;
					current->right = current->right->right;
					DeleteIndexNode((IndexNode*)(current->father), current);
				}
				else if (current->father == current->left->father) {	//左节点的父节点与当前节点的父节点相同。
					//找到父节点相应的数据，将其移到左节点的尾部
					for (; i < father->num; i++)
						if (father->childs[i] == current->left) break;
					current->left->index[current->left->num++] = father->index[i];
					//与左节点合并，注意合并当前节点的儿子节点并且当前右节点儿子节点的父节点
					int i = 0;
					for (int i = 0; i < current->num; i++) {
						((IndexNode*)(current->left))->childs[current->left->num] = current->childs[i];
						((IndexNode*)(current->left))->childs[current->left->num]->father = current->left;
						current->left->index[current->left->num++] = current->index[i];
					}
					((IndexNode*)(current->left))->childs[current->left->num] = current->childs[i];
					((IndexNode*)(current->left))->childs[current->left->num]->father = current->left;
					//注意修改这两个节点之间连接关系
					if (current->right)current->right->left = current->left;
					current->left->right = current->right;
					DeleteIndexNode((IndexNode*)(current->father), current->left);
				}
			}
		}
	}
}

void BPlusChange(int data) {
	//string* p = BPlusFind(data);
	//对数据进行修改
}

void BPlusFind(int data) {
	Node* t = root;
	while (t->type != 2) {
		for (int i = 0; i <= t->num; i++) {
			if (i == t->num || data < t->index[i]) {
				IndexNode* p = (IndexNode*)t;
				t = p->childs[i];
				break;
			}
		}
	}
	for (int i = 0; i < t->num; i++) {
		if (t->index[i] == data) {
			DataNode* p = (DataNode*)t;
			//return &p->data[i];
			return;
		}
	}
}

void BPlusPrint() {
	if (root->type == 1) {
		deque<Node*> S;
		S.push_back(root);
		int num =0 , j = 1;
		while (!S.empty()) {
			Node* p = S.front();
			S.pop_front();
			j--;
			if (p->type == 1) {
				num += p->num + 1;
				for (int i = 0; i <= p->num; i++) {
					S.push_back(((IndexNode*)p)->childs[i]);
				}
				for (int i = 0; i < p->num; i++)cout << p->index[i] << " ";
				cout << "\t";
			}
			else {
				for (int i = 0; i < p->num; i++)cout << p->index[i] << " ";
				cout << "\t";
			}
			if (j == 0) {
				cout << endl;
				j = num;
				num = 0;
			}
		}
	}
	else {
		for (int i = 0; i < root->num; i++)cout << root->index[i] << " ";
		cout << endl;
	}
}