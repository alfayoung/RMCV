#include <bits/stdc++.h>
using namespace std;
const int maxn = 5005;
#define d(x) cerr << #x << " = " << x << endl

// time complexity and memory complexity O(size of output)
// I was considering of writing LinkCutTree but it's not efficient
// The basic idea is to maintain a Tree structure with Lists and Stacks

int pos[maxn]; // Tree id -> List id
char s[maxn][maxn];
int topUndo, topRedo;
class Interface {
private:
	// definition of tree node
	struct List {
		List() {}
		int head, tail, tot;
		int recy[maxn], topRecy; // memory recycle
		struct Node {
			int pre, nxt, id;
		} p[maxn];
		void init() {
			head = topRecy ? recy[topRecy--] : ++tot;
			tail = topRecy ? recy[topRecy--] : ++tot;
			p[head].nxt = tail; p[head].id = 0;
			p[tail].pre = head; p[tail].id = 0;
		}
		// insert x after t
		void insert(int t, int x) {
			int tt = topRecy ? recy[topRecy--] : ++tot;
			p[tt].id = x; pos[x] = tt;
			p[p[t].nxt].pre = tt;
			p[tt].nxt = p[t].nxt;
			p[t].nxt = tt;
			p[tt].pre = t;
		}
		void remove(int t) {
			recy[++topRecy] = t;
			pos[p[t].id] = 0;
			p[p[t].pre].nxt = p[t].nxt;
			p[p[t].nxt].pre = p[t].pre;
		}
	};
	// tree node
	struct node : List {
		char type;
		int fa;
		int lenx, leny;
		node() { type = '0'; }
		// inherit List to store sons
	} tree[maxn];
	struct binCreate {
		int x, y, yPre; char type;
	};
	struct Q {
		string cmd;
		int xCreate; // create
		vector<binCreate> bin; int xDel; // del
		int xMove, yMove, xPre, xFa; // move
		int xSet; char tSet; // set
	} stkUndo[maxn], stkRedo[maxn];
public:
	// O(1) create
	void create(int x, int y, char t) {
		if (x == y) {
			cout << "Indices are identical!" << endl;
			return ;
		}
		if (pos[x]) {
			cout << "Index " << x << " already exists!" << endl;
			return ;
		}
		if (y && !pos[y]) {
			cout << "Index " << y << " not exist!" << endl;
			return ;
		}
		
		topRedo = 0;
		topUndo++;
		stkUndo[topUndo].cmd = "create";
		stkUndo[topUndo].xCreate = x;
		
		tree[y].insert(tree[y].p[tree[y].tail].pre, x);
		tree[x].init();
		tree[x].fa = y;
		tree[x].type = t;
	}
	// O(number of nodes to delete) del
	void delRecursive(int u) {
		binCreate tmp;
		tmp.x = tree[u].fa; tmp.y = u;
		tmp.yPre = tree[tree[u].fa].p[pos[u]].pre;
		tmp.yPre = tree[tree[u].fa].p[tmp.yPre].id;
		tmp.type = tree[u].type;
		stkUndo[topUndo].bin.push_back(tmp);
		
		for (int e = tree[u].p[tree[u].head].nxt; e != tree[u].tail; e = tree[u].p[e].nxt) {
			int v = tree[u].p[e].id;
			delRecursive(v);
		}
		tree[tree[u].fa].remove(pos[u]);
	}
	void del(int x) {
		if (!pos[x]) return ;
		
		topRedo = 0;
		topUndo++; stkUndo[topUndo].cmd = "del";
		stkUndo[topUndo].bin.clear();
		
		delRecursive(x);
	}
	// O(1) move
	void move(int x, int y) {
		if ((x && !pos[x]) || (y && !pos[y])) {
			cout << "Index not found!" << endl;
			return ;
		}
		for (int u = y; u; u = tree[u].fa) if (u == x) {
			cout << "Disorder in hierarchy!" << endl;
			return ;
		}
		
		topRedo = 0;
		topUndo++;
		stkUndo[topUndo].cmd = "move";
		stkUndo[topUndo].xMove = x;
		stkUndo[topUndo].yMove = y;
		stkUndo[topUndo].xFa = tree[x].fa;
		stkUndo[topUndo].xPre = tree[tree[x].fa].p[pos[x]].pre;
		stkUndo[topUndo].xPre = tree[tree[x].fa].p[stkUndo[topUndo].xPre].id;
		
		tree[tree[x].fa].remove(pos[x]);
		tree[x].fa = y;
		tree[y].insert(tree[y].p[tree[y].tail].pre, x);
	}
	// O(1) set
	void set(int x, char t) {
		if (!pos[x]) {
			cout << "Index not exist!" << endl;
			return ;
		}
		
		topRedo = 0;
		topUndo++;
		stkUndo[topUndo].cmd = "set";
		stkUndo[topUndo].xSet = x;
		stkUndo[topUndo].tSet = tree[x].type;
		
		tree[x].type = t;
	}
	void undo() {
		if (!topUndo) {
			cout << "Illegal Undo!" << endl;
			return ;
		}
		Q tmp = stkUndo[topUndo--];
		if (tmp.cmd == "create") {
			topRedo++;
			stkRedo[topRedo].cmd = "create";
			stkRedo[topRedo].xCreate = tmp.xCreate;
			stkRedo[topRedo].xFa = tree[tmp.xCreate].fa;
			stkRedo[topRedo].tSet = tree[tmp.xCreate].type;
			
			tree[tree[tmp.xCreate].fa].remove(pos[tmp.xCreate]);
		} else if (tmp.cmd == "del") {
			topRedo++;
			stkRedo[topRedo].cmd = "del";
			stkRedo[topRedo].xDel = tmp.bin[0].y;
			
			int flag = 1;
			for (binCreate op : tmp.bin) {
				tree[op.y].init();
				tree[op.y].type = op.type;
				tree[op.y].fa = op.x;
				if (flag) tree[op.x].insert(op.yPre ? pos[op.yPre] : tree[op.x].head, op.y);
				else tree[op.x].insert(tree[op.x].p[tree[op.x].tail].pre, op.y);
				flag = 0;
			}
		} else if (tmp.cmd == "move") {
			topRedo++;
			stkRedo[topRedo].cmd = "move";
			stkRedo[topRedo].xMove = tmp.xMove;
			stkRedo[topRedo].yMove = tmp.yMove;
			
			tree[tmp.yMove].remove(pos[tmp.xMove]);
			tree[tmp.xMove].fa = tmp.xFa;
			tree[tmp.xFa].insert(tmp.xPre ? pos[tmp.xPre] : tree[tmp.xFa].head, tmp.xMove);
		} else if (tmp.cmd == "set") {
			topRedo++;
			stkRedo[topRedo].cmd = "set";
			stkRedo[topRedo].xSet = tmp.xSet;
			stkRedo[topRedo].tSet = tree[tmp.xSet].type;
			
			tree[tmp.xSet].type = tmp.tSet;
		}
	}
	void redo() {
		if (!topRedo) {
			cout << "Illegal Redo!" << endl;
			return ;
		}
		Q tmp = stkRedo[topRedo--];
		int mask = topRedo;
		if (tmp.cmd == "create") {
			create(tmp.xCreate, tmp.xFa, tmp.tSet);
		} else if (tmp.cmd == "del") {
			del(tmp.xDel);
		} else if (tmp.cmd == "move") {
			move(tmp.xMove, tmp.yMove);
		} else if (tmp.cmd == "set") {
			set(tmp.xSet, tmp.tSet);
		}
		topRedo = mask;
	}
	
	// print the layers
	void getMaxDepth(int u) {
		int dlen = 0; for (int tmp = u; tmp; tmp /= 10) dlen++;
		if (!u) dlen = 1;
		tree[u].lenx = 3; tree[u].leny = 2 + dlen;
		for (int e = tree[u].p[tree[u].head].nxt; e != tree[u].tail; e = tree[u].p[e].nxt) {
			int v = tree[u].p[e].id;
			getMaxDepth(v);
			tree[u].leny = max(tree[u].leny, tree[v].leny + 2);
			tree[u].lenx = tree[u].lenx + tree[v].lenx;
		}
		if ((dlen & 1) && !(tree[u].leny & 1)) tree[u].leny++;
		else if (!(dlen & 1) && (tree[u].leny & 1)) tree[u].leny++;
	}
	void dfsShow(int u, int Lx, int Ly) {
		int lenx = tree[u].lenx, leny = tree[u].leny; char t = tree[u].type;
		s[Lx][Ly] = s[Lx + lenx - 1][Ly] = s[Lx][Ly + leny - 1] = s[Lx + lenx - 1][Ly + leny - 1] = t;
		int dlen = 0; for (int tmp = u; tmp; tmp /= 10) dlen++;
		if (!u) s[Lx + 1][Ly + leny / 2] = '0';
		for (int tmp = u, i = 0; tmp; tmp /= 10, i++) {
			s[Lx + 1][Ly + leny / 2 + (dlen - 1) / 2 - i] = (char)((tmp % 10) + '0');
		}
		for (int i = Ly + 1; i <= Ly + leny - 2; i++) {
			s[Lx][i] = '-';
			s[Lx + lenx - 1][i] = '-';
		}
		for (int i = Lx + 1; i <= Lx + lenx - 2; i++) {
			s[i][Ly] = '|';
			s[i][Ly + leny - 1] = '|';
		}
		Lx += 2; Ly++;
		for (int e = tree[u].p[tree[u].head].nxt; e != tree[u].tail; e = tree[u].p[e].nxt) {
			int v = tree[u].p[e].id;
			dfsShow(v, Lx, Ly);
			Lx += tree[v].lenx;
		}
	}
	void show() {
		memset(s, 0, sizeof(s));
		getMaxDepth(0);
		dfsShow(0, 1, 1);
		for (int i = 1; i <= tree[0].lenx; i++) {
			for (int j = 1; j <= tree[0].leny; j++) cout << s[i][j];
			cout << endl;
		}
	}
} B;

int main() {
	for (;;) {
		string cmd; cin >> cmd;
		if (cmd == "create") {
			int x, y; string t; char type;
			cin >> x >> y >> t;
			if (x <= 0 || x > 5000 || y < 0 || y > 5000) {
				cout << "Index Inapplicable!" << endl;
				continue;
			}
			if (t[1] == 'r') type = '+';
			else if (t[1] == 'c') type = 'o';
			else if (t[1] == 'l') type = '*';
			else { cout << "Invalid Type!" << endl; continue; }
			B.create(x, y, type);
		} else if (cmd == "del") {
			int x; cin >> x;
			if (x <= 0 || x > 5000) { cout << "Index Inapplicable!" << endl; continue; }
			B.del(x);
		} else if (cmd == "mv") {
			int x, y; cin >> x >> y;
			if (x <= 0 || x > 5000 || y < 0 || y > 5000) {
				cout << "Index Inapplicable!" << endl;
				continue;
			}
			B.move(x, y);
		} else if (cmd == "set") {
			int x; string t; char type;
			cin >> x >> t;
			if (x <= 0 || x > 5000) {
				cout << "Index Inapplicable!" << endl;
				continue;
			}
			if (t[1] == 'r') type = '+';
			else if (t[1] == 'c') type = 'o';
			else if (t[1] == 'l') type = '*';
			else { cout << "Invalid Type!" << endl; continue; }
			B.set(x, type);
		} else if (cmd == "undo") {
			B.undo();
		} else if (cmd == "redo") {
			B.redo();
		} else {
			while (getchar() != '\n');
			cout << "Invalid Command!" << endl;
			continue;
		}
		B.show();
	}
	return 0;
}
