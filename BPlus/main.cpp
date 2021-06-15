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
	//���ݼ���Ϊstring;
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
		cout << "ɾ��:";
		cin >> d;
		BPlusDelete(d);
		BPlusPrint();
	}
}

void BPlusCreate() {
	int d;
	cout << "����B+��(0����β):" << endl;
	while (cin>>d) {
		if (d==0)break;
		BPlusAdd(d);
	}
	BPlusPrint();
}

void BPlusAdd(int data) {
	if (root == NULL) {				//��û���κνڵ�ʱ������һ�����ݽڵ㣬Ȼ���������
		root = new DataNode();
		root->type = 2;
		AddDataNode((DataNode *)root, data);
	}
	else if (root->type==2) {		//��ֻ�����ݽڵ�ʱ����ýڵ��������
		AddDataNode((DataNode*)root, data);
	}		
	else {									//�����ڵ��������ڵ�ʱ��ѭ���ݹ���ҵ���Ҫ������ݵ����ݽڵ㣬Ȼ���������
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
	//�Ƚ�Ҫ��ӵ����ݷ��ڽڵ����Ȼ��Խڵ㵱ǰ���ݽ���һ��ð�����򣬽��������ݷ��ں��ʵ�λ��
	current->index[current->num] = data;			
	for (int i = current->num; i > 0 && current->index[i] < current->index[i - 1]; i--) {
		swap(current->index[i], current->index[i - 1]);
	}
	current->num++;
	//�����ǰ�ڵ����ݸ�������B+���Ľ��������з���
	if (current->num == degree) {
		//�½�һ���ҽڵ㣬�����ѳ�ȥ�����ݷ����ҽڵ㣬�������ݷ���ԭ�ڵ㲻��
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
		//��û�и��ڵ㣬��B+����ֻ��һ�����ݽڵ�
		if (current->father == NULL) {
			IndexNode* father = new IndexNode();
			father->type = 1;
			root = father;		//���ڵ��������ɵĸ��ڵ�
			current->father = father;
			father->childs[0] = current;
			AddIndexNode(father, current, current->right->index[0]);
		}
		else {	//��ǰ�ڵ��и��ڵ�
			AddIndexNode((IndexNode*)(current->father), current, current->right->index[0]);
		}
	}
}

void AddIndexNode(IndexNode * current, Node* child,int data) {
	//�������ɵ��ҽڵ�ĸ��ڵ�ָ��ǰ�ڵ㣬���Ѹýڵ��������ŷ��ڵ�ǰ�ڵ���󣬲�ʹ��һ��ð�����򣬽�����ں���λ��
	child->right->father = current;
	current->index[current->num] = data;
	current->childs[current->num + 1] = child->right;
	for (int i = current->num; i > 0 && current->index[i] < current->index[i - 1]; i--) {
		swap(current->index[i], current->index[i - 1]);
		swap(current->childs[i+1], current->childs[i]);
	}
	current->num++;
	//�����ǰ�ڵ����ݸ�������B+���Ľ��������з���
	if (current->num == degree) {
		//�½�һ���ҽڵ㣬�����ѳ�ȥ�����ݷ����ҽڵ㣬�������ݷ���ԭ�ڵ㲻��,������Ҫ���ķ��ѳ�ȥ�������ڵ��¶��ӽڵ�ĸ�����
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
		//��û�и��ڵ�
		if (current->father == NULL) {
			IndexNode* father = new IndexNode();
			father->type = 1;
			root = father;		//�ı���ڵ�Ϊ�½����ڵ�
			current->father = father;
			father->childs[0] = current;
			AddIndexNode(father, current, current->index[current->num]);
		}
		else {		//�и��ڵ�
			AddIndexNode((IndexNode*)(current->father), current, current->index[current->num]);
		}
	}
}

void BPlusDelete(int data) {
	//ѭ���ݹ���ҵ���Ҫɾ�����ݵ����ݽڵ㣬Ȼ��ɾ������
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
	//�Ե�ǰ�ڵ���б������ҽ�Ҫɾ�������ݵģ�����ҵ��ˣ������������ν�һ�����û���ҵ�����ӡ������Ȼ���˳�
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
	if (isExist) {	//���ɾ���ɹ������ݵ�ǰ�ڵ������ݸ����������Ƿ����ֵܽڵ�ϲ�
		cout << "ɾ���ɹ�" << endl;
		if (current->num < (degree - 1) / 2) {
			if (current->left!=NULL&& current->left->num > (degree - 1) / 2) {	//ѡ�����ֵܽڵ����
				//����ǰ�ڵ��е�����������һ��������λ��Ȼ�����ֵܽڵ��������ݷŵ���ǰ�ڵ��һ
				for (int i = current->num; i > 0; i--)current->index[i] = current->index[i - 1];
				current->index[0] = current->left->index[current->left->num-1];
				current->num++;
				current->left->num--;
				//�޸ĸ��ڵ㡣�ҵ���ǰ�ڵ����ڵ㹲ͬ�ĸ��ڵ㣬������ڵ��ڸ��ڵ�Ķ��ӽڵ����λ��ȷ��Ҫ�޸����ݵ�λ��
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
			else if (current->right != NULL && current->right->num > (degree - 1) / 2) { //ѡ�����ֵܽڵ����
				//�����ֵܽڵ�ĵ�һ�����ݷŵ���ǰ�ڵ���󣬲������ֵܽڵ��е��������ν�һ�����λ
				current->index[current->num] = current->right->index[0];
				current->num++;
				current->right->num--;
				for (int i = 0; i < current->right->num; i++)current->right->index[i] = current->right->index[i + 1];
				//�޸ĸ��ڵ㡣�ҵ���ǰ�ڵ���ҽڵ㹲ͬ�ĸ��ڵ㣬������ڵ��ڸ��ڵ�Ķ��ӽڵ����λ��ȷ��Ҫ�޸����ݵ�λ��
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
			else {	//���кϲ�
				if (current->right == NULL && current->left == NULL) { return; }	//�����ǰ�ڵ�����ҽڵ㶼�ǿգ�ֱ���˳�
				else if(current->left==NULL){																//�����ڵ�Ϊ�գ�ѡ�����ҽڵ�ϲ�
					for (int i = 0; i < current->right->num; i++)current->index[current->num++] = current->right->index[i];
					if (current->right->right)current->right->right->left = current;	//ע���޸��������ڵ�֮�����ӹ�ϵ
					current->right = current->right->right;
					DeleteIndexNode((IndexNode*)(current->father), current);
				}
				else if(current->right==NULL){															//����ҽڵ�Ϊ�գ�ѡ������ڵ�ϲ�
					for (int i = 0; i < current->num; i++)current->left->index[current->left->num++] = current->index[i];
					if (current->right)current->right->left = current->left;					//ע���޸��������ڵ�֮�����ӹ�ϵ
					current->left->right = current->right;
					DeleteIndexNode((IndexNode*)(current->father), current->left);
				}
				else {																									//���ҽڵ㶼���ڣ�ѡ���������ڵ�ĸ��ڵ���ͬ��(��)�ҽڵ���кϲ������ںϲ�
					if (current->father == current->right->father) {							//�ҽڵ�ĸ��ڵ��뵱ǰ�ڵ�ĸ��ڵ���ͬ��
						for (int i = 0; i < current->right->num; i++)current->index[current->num++] = current->right->index[i];
						if (current->right->right)current->right->right->left = current;	//ע���޸��������ڵ�֮�����ӹ�ϵ
						current->right = current->right->right;
						DeleteIndexNode((IndexNode*)(current->father), current);
					}else if(current->father == current->left->father) {						//��ڵ�ĸ��ڵ��뵱ǰ�ڵ�ĸ��ڵ���ͬ��
						for (int i = 0; i < current->num; i++)current->left->index[current->left->num++] = current->index[i];
						if (current->right)current->right->left = current->left;				//ע���޸��������ڵ�֮�����ӹ�ϵ
						current->left->right = current->right;
						DeleteIndexNode((IndexNode*)(current->father), current->left);
					}
				}
			}
		}
	}
	else {
		cout << "������" << endl;
		return;
	}
}

void DeleteIndexNode(IndexNode* current, Node* child) {
	//�Ե�ǰ�ڵ���б������ҽ�Ҫɾ�������ݵģ�����child�ڵ��ڵ�ǰ�ڵ�Ķ��ӽڵ��λ�ã����ҵ��󣬺����������ν�һ
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
	//���ݵ�ǰ�ڵ������ݸ����������Ƿ����ֵܽڵ�ϲ�
	if (current->num < (degree - 1) / 2) {
		if (current->left != NULL && current->left->num > (degree - 1) / 2) {	//ѡ�����ֵܽڵ����
			for (int i = current->num; i > 0; i--) {
				current->index[i] = current->index[i - 1];
				current->childs[i+1] = current->childs[i];			//ע�����ڵ�Ķ��ӽڵ�����һ����λ
			}
			current->childs[1] = current->childs[0];					
			current->num++;
			//�޸ĸ��ڵ㡣�ҵ���ǰ�ڵ����ڵ㹲ͬ�ĸ��ڵ㣬������ڵ��ڸ��ڵ�Ķ��ӽڵ����λ��ȷ��Ҫ�޸����ݵ�λ��
			Node* left = current->left, * right = current;
			while (left->father != right->father) {
				left = left->father;
				right = right->father;
			}
			IndexNode* father = (IndexNode*)(left->father);
			for (int i = 0; i < father->num; i++) {
				if (father->childs[i] == left) {
					current->index[0] = father->index[i];		//�����ڵ���Ӧ���ݷ��ڵ�ǰ�ڵ���λ
					current->childs[0] = ((IndexNode*)(current->left))->childs[current->left->num];	//��ڵ������ӽڵ�ŵ���ǰ�ڵ���λ
					father->index[i] = current->left->index[--current->left->num];		////��ڵ���������Ƶ����ڵ�
					break;
				}
			}
		}
		else if (current->right != NULL && current->right->num > (degree - 1) / 2) { //ѡ�����ֵܽڵ����
			int temp = current->right->index[0];
			Node* tempnode = ((IndexNode*)(current->right))->childs[0];
			for (int i = 0; i < current->right->num-1; i++) {
				current->right->index[i] = current->right->index[i + 1];
				((IndexNode*)(current->right))->childs[i] = ((IndexNode*)(current->right))->childs[i + 1];			//ע���ҽڵ���׸����ӽڵ�ҲҪ�Ƶ���ǰ�ڵ�
			}
			((IndexNode*)(current->right))->childs[current->right->num-1] = ((IndexNode*)(current->right))->childs[current->right->num];
			current->right->num--;
			//�޸ĸ��ڵ㡣�ҵ���ǰ�ڵ���ҽڵ㹲ͬ�ĸ��ڵ㣬������ڵ��ڸ��ڵ�Ķ��ӽڵ����λ��ȷ��Ҫ�޸����ݵ�λ��
			Node* left = current, * right = current->right;
			while (left->father != right->father) {
				left = left->father;
				right = right->father;
			}
			IndexNode* father = (IndexNode*)(left->father);
			for (int i = 0; i < father->num; i++) {
				if (father->childs[i] == left) {
					current->childs[current->num] = tempnode;		//��ǰ�ڵ�Ķ��ӽڵ��β������ҽڵ���ӽڵ���׸�
					current->index[current->num++] = father->index[i];	//�����ڵ���Ӧ���ݷ��ڵ�ǰ�ڵ�β��
					father->index[i] = temp;											//�ҽڵ���λ�����Ƶ����ڵ�
					break;
				}
			}
		}
		else {
			IndexNode* father = (IndexNode*)(current->father);
			int i = 0;
			if (current->right == NULL && current->left == NULL) {		//�����ǰ�ڵ�����ҽڵ㶼�ǿգ��ҵ�ǰ�ڵ������ݣ�ѡ����Ψһ�Ķ��ӽڵ������ڵ�
				if (current->num == 0) { root = current->childs[0]; delete current->father; }
			}
			else if (current->left == NULL) {		//�����ڵ�Ϊ�գ�ѡ�����ҽڵ�ϲ�
				//�ҵ����ڵ���Ӧ�����ݣ������Ƶ���ǰ�ڵ��β��
				for (; i < father->num; i++)
					if (father->childs[i] == current) break;
				current->index[current->num++] = father->index[i];		
				//���ҽڵ�ϲ���ע��ϲ��ҽڵ�Ķ��ӽڵ㲢���޸��ҽڵ���ӽڵ�ĸ��ڵ�
				int i = 0;
				for (; i < current->right->num; i++) {									
					current->childs[current->num] = ((IndexNode*)(current->right))->childs[i];
					current->childs[current->num]->father = current;
					current->index[current->num++] = current->right->index[i];
				}
				current->childs[current->num] = ((IndexNode*)(current->right))->childs[i];
				current->childs[current->num]->father = current;
				//ע���޸��������ڵ�֮�����ӹ�ϵ
				if (current->right->right)current->right->right->left = current;	
				current->right = current->right->right;
				DeleteIndexNode((IndexNode*)(current->father), current);
			}
			else if (current->right == NULL) {		//����ҽڵ�Ϊ�գ�ѡ������ڵ�ϲ�
				//�ҵ����ڵ���Ӧ�����ݣ������Ƶ���ڵ��β��
				for (; i < father->num; i++)
					if (father->childs[i] == current->left) break;
				current->left->index[current->left->num++] = father->index[i];
				//����ڵ�ϲ���ע��ϲ���ǰ�ڵ�Ķ��ӽڵ㲢�ҵ�ǰ�ҽڵ���ӽڵ�ĸ��ڵ�
				int i = 0;
				for (; i < current->num; i++) {
					((IndexNode*)(current->left))->childs[current->left->num] = current->childs[i];
					((IndexNode*)(current->left))->childs[current->left->num]->father = current->left;
					current->left->index[current->left->num++] = current->index[i];
				}
				((IndexNode*)(current->left))->childs[current->left->num] = current->childs[i];
				((IndexNode*)(current->left))->childs[current->left->num]->father = current->left;
				//ע���޸��������ڵ�֮�����ӹ�ϵ
				if (current->right)current->right->left = current->left;
				current->left->right = current->right;
				DeleteIndexNode((IndexNode*)(current->father), current->left);
			}
			else {		//���ҽڵ㶼���ڣ�ѡ���������ڵ�ĸ��ڵ���ͬ��(��)�ҽڵ���кϲ������ںϲ�
				if (current->father == current->right->father) {	//�ҽڵ�ĸ��ڵ��뵱ǰ�ڵ�ĸ��ڵ���ͬ��
					//�ҵ����ڵ���Ӧ�����ݣ������Ƶ���ǰ�ڵ��β��
					for (; i < father->num; i++)
						if (father->childs[i] == current) break;
					current->index[current->num++] = father->index[i];
					//���ҽڵ�ϲ���ע��ϲ��ҽڵ�Ķ��ӽڵ㲢���޸��ҽڵ���ӽڵ�ĸ��ڵ�
					int i = 0;
					for (; i < current->right->num; i++) {
						current->childs[current->num] = ((IndexNode*)(current->right))->childs[i];
						current->childs[current->num]->father = current;
						current->index[current->num++] = current->right->index[i];
					}
					current->childs[current->num] = ((IndexNode*)(current->right))->childs[i];
					current->childs[current->num]->father = current;
					//ע���޸��������ڵ�֮�����ӹ�ϵ
					if (current->right->right)current->right->right->left = current;
					current->right = current->right->right;
					DeleteIndexNode((IndexNode*)(current->father), current);
				}
				else if (current->father == current->left->father) {	//��ڵ�ĸ��ڵ��뵱ǰ�ڵ�ĸ��ڵ���ͬ��
					//�ҵ����ڵ���Ӧ�����ݣ������Ƶ���ڵ��β��
					for (; i < father->num; i++)
						if (father->childs[i] == current->left) break;
					current->left->index[current->left->num++] = father->index[i];
					//����ڵ�ϲ���ע��ϲ���ǰ�ڵ�Ķ��ӽڵ㲢�ҵ�ǰ�ҽڵ���ӽڵ�ĸ��ڵ�
					int i = 0;
					for (int i = 0; i < current->num; i++) {
						((IndexNode*)(current->left))->childs[current->left->num] = current->childs[i];
						((IndexNode*)(current->left))->childs[current->left->num]->father = current->left;
						current->left->index[current->left->num++] = current->index[i];
					}
					((IndexNode*)(current->left))->childs[current->left->num] = current->childs[i];
					((IndexNode*)(current->left))->childs[current->left->num]->father = current->left;
					//ע���޸��������ڵ�֮�����ӹ�ϵ
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
	//�����ݽ����޸�
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