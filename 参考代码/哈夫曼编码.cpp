﻿#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

struct HuffmanNode
{
    int index;
    int weight;
    int parent, lchild, rchild;
};
// 定义结点的排序规则，sort算法需要使用这个重载的运算符
bool operator<(const HuffmanNode &node1, const HuffmanNode &node2)
{
    return node1.weight < node2.weight;
}

struct HuffmanTree
{
    HuffmanNode *nodes;
    int leafnumber;
};

// 在哈夫曼树[0~n-1]范围内查找根结点权值最小的k1和根结点权值次小的k2
int select(HuffmanTree &T, int n, int &k1, int &k2)
{
    int i1 = -1, i2 = -2;
    for (int i = 0; i < n; i++)
    {
        // 略过非根结点
        if (T.nodes[i].parent != -1)
            continue;
        if (i1 == -1 || T.nodes[i].weight < T.nodes[i1].weight)
        {
            // 发现一个比目前已知权值最小的i1更小的根结点i
            // 原来的最小变成了次小
            i2 = i1;
            i1 = i;
        }
        else if (i2 == -1 || T.nodes[i].weight < T.nodes[i2].weight)
        {
            // 发现一个比目前已知权值次小的i2更小的根结点i
            i2 = i;
        }
    }

    // 约定先出现的（低下标）在左子
    k1 = min(i1, i2);
    k2 = max(i1, i2);
    return 0;
}

// 采用顺序查找权值最小次小根结点的策略构造哈夫曼树
int CreateHuffmanTree2(HuffmanTree &T, int leafnumber, int *weights)
{
    // 分配哈夫曼树存储空间
    T.nodes = new HuffmanNode[leafnumber + leafnumber - 1];
    T.leafnumber = leafnumber;

    // 初始化叶结点
    for (int i = 0; i < leafnumber; i++)
    {
        T.nodes[i].index = i;
        T.nodes[i].weight = weights[i];
        T.nodes[i].parent = T.nodes[i].lchild = T.nodes[i].rchild = -1;
    }

    // 开始构造哈夫曼树
    for (int i = 0; i < leafnumber - 1; i++)
    {
        // 选择权值最小和次小的根结点k1, k2，且k1 < k2
        int k1, k2;
        select(T, leafnumber + i, k1, k2);

        // 将根结点k1, 根结点k2合并到新增根结点i + leafnumber下
        T.nodes[i + leafnumber].weight = T.nodes[k1].weight + T.nodes[k2].weight;
        T.nodes[i + leafnumber].parent = -1;
        T.nodes[i + leafnumber].lchild = k1;
        T.nodes[i + leafnumber].rchild = k2;
        T.nodes[k1].parent = i + leafnumber;
        T.nodes[k2].parent = i + leafnumber;
    }

    return 0;
}

// 采用对叶结点预排序策略构造哈夫曼树
int CreateHuffmanTree(HuffmanTree &T, int leafnumber, int *weights)
{
    // 分配哈夫曼树存储空间
    T.nodes = new HuffmanNode[leafnumber + leafnumber - 1];
    T.leafnumber = leafnumber;

    // 初始化叶结点
    for (int i = 0; i < leafnumber; i++)
    {
        T.nodes[i].index = i;
        T.nodes[i].weight = weights[i];
        T.nodes[i].parent = T.nodes[i].lchild = T.nodes[i].rchild = -1;
    }

    // 将叶结点按权值排序
    sort(T.nodes, T.nodes + leafnumber);

    // 开始构造哈夫曼树
    int s = 0, t = leafnumber; // 分别用来遍历叶结点和非叶结点
    for (int i = 0; i < leafnumber - 1; i++)
    {
        // 选择第一个权值最小的根结点
        int k1;
        if (s >= leafnumber)
            k1 = t++;
        else if (t >= leafnumber + i)
            k1 = s++;
        else if (T.nodes[s].weight < T.nodes[t].weight)
            k1 = s++;
        else
            k1 = t++;

        // 选择第二个权值最小的根结点
        int k2;
        if (s < leafnumber && (t >= leafnumber + i || T.nodes[s].weight < T.nodes[t].weight))
            k2 = s++;
        else
            k2 = t++;

        // 将根结点k1, 根结点k2合并到新增根结点i + leafnumber下
        T.nodes[i + leafnumber].weight = T.nodes[k1].weight + T.nodes[k2].weight;
        T.nodes[i + leafnumber].parent = -1;
        T.nodes[i + leafnumber].lchild = k1;
        T.nodes[i + leafnumber].rchild = k2;
        T.nodes[k1].parent = i + leafnumber;
        T.nodes[k2].parent = i + leafnumber;
    }

    return 0;
}

int DestroyHuffmanTree(HuffmanTree &T)
{
    delete[] T.nodes;
    T.nodes = nullptr;
    T.leafnumber = 0;
    return 0;
}

// 自底向上构造哈夫曼编码
vector<string> CreateHuffmanCode(HuffmanTree &T)
{
    vector<string> aCodes(T.leafnumber);
    char *buffer = new char[T.leafnumber]{};
    // 依次从每一个叶结点循父链(parent)寻根
    for (int i = 0; i < T.leafnumber; i++)
    {
        // t是编码记录指针，从buffer的尾部开始从后向前记录
        int t = T.leafnumber - 1;
        for (int j = i, k = T.nodes[i].parent; k >= 0; j = k, k = T.nodes[k].parent)
        {
            buffer[--t] = (j == T.nodes[k].lchild ? '0' : '1');
        }
        // 到达根结点后，将得到的编码存入aCodes中
        aCodes[T.nodes[i].index] = buffer + t;
    }
    delete[] buffer;
    return aCodes;
}

// 自顶向下构造哈夫曼编码的遍历过程，i表示当前结点的下标，j表示当前编码串的位置
int _CreateHuffmanCode(vector<string> &aCodes, HuffmanTree &T, int i, char buffer[], int j)
{
    // 遍历到叶结点时即得到该叶结点的编码，将编码存入aCodes中
    if (i < T.leafnumber)
    {
        buffer[j] = '\0';
        aCodes[T.nodes[i].index] = buffer;
        return 0;
    }
    // 遍历左子树
    buffer[j] = '0';
    _CreateHuffmanCode(aCodes, T, T.nodes[i].lchild, buffer, j + 1);
    // 遍历右子树
    buffer[j] = '1';
    _CreateHuffmanCode(aCodes, T, T.nodes[i].rchild, buffer, j + 1);
    return 0;
}

// 自顶向下构造哈夫曼编码主过程
vector<string> CreateHuffmanCode2(HuffmanTree &T)
{
    vector<string> aCodes(T.leafnumber);
    char *buffer = new char[T.leafnumber]{};
    // 从哈夫曼树的根结点开始遍历
    _CreateHuffmanCode(aCodes, T, T.leafnumber + T.leafnumber - 2, buffer, 0);
    delete[] buffer;
    return aCodes;
}

string Encode(string aPlaneText, const vector<string> &aCodes)
{
    string aEncodedText;
    for (char ch : aPlaneText)
    {
        aEncodedText += aCodes[ch - 'a']; // 这里假定字符为a~e，且没有考虑出现非法字符的情况
    }
    return aEncodedText;
}

string Decode(string aEncodedText, HuffmanTree &T)
{
    string aDecodedText;
    int i = T.leafnumber + T.leafnumber - 2; // 指向根结点
    for (char ch : aEncodedText)
    {
        i = ch == '0' ? T.nodes[i].lchild : T.nodes[i].rchild;
        if (i < T.leafnumber)
        {
            aDecodedText += 'a' + T.nodes[i].index; // 假设对应字符为a~e
            i = T.leafnumber + T.leafnumber - 2;
        }
    }
    return aDecodedText;
}

int main()
{
    HuffmanTree T;
    int weights[] = {13, 7, 8, 6, 21, 3, 9, 2}; // 假设对应字符为a~e

    cout << "创建哈夫曼树……" << endl;
    CreateHuffmanTree(T, sizeof(weights) / sizeof(*weights), weights);

    cout << "自顶向下创建哈夫曼编码……" << endl;
    vector<string> aCodes0 = CreateHuffmanCode2(T);
    for (int i = 0; i < aCodes0.size(); i++)
    {
        // 假设字符为a~e
        cout << (char)('a' + i) << ": " << aCodes0[i] << endl;
    }

    cout << "自底向上创建哈夫曼编码……" << endl;
    vector<string> aCodes = CreateHuffmanCode(T);
    for (int i = 0; i < aCodes.size(); i++)
    {
        // 假设字符为a~e
        cout << (char)('a' + i) << ": " << aCodes[i] << endl;
    }

    string aPlaneText{"ahfbaegecdafbec"};
    cout << "对 " << aPlaneText << " 进行编码……" << endl;
    string aEncodedText = Encode(aPlaneText, aCodes);
    cout << aPlaneText << " 编码结果为 " << aEncodedText << endl;

    cout << "对 " << aEncodedText << " 进行解码……" << endl;
    string aDecodedText = Decode(aEncodedText, T);
    cout << aEncodedText << " 解码结果为 " << aDecodedText << endl;

    DestroyHuffmanTree(T);
    system("pause");
    return 0;
}