#include <iostream>
#include <queue>
#include <fstream>
#include <string>
#include <map>
#include <sstream>
#include <cstring>

using namespace std;


const int LETTERCOUNT = 26;


struct Node
{
    char letter;
    int num;
    Node *left, *right;
};

Node* getNode(char c, int num, Node* left, Node* right)
{
    Node* node = new Node();
    node->letter = c;
    node->num = num;
    node->left = left;
    node->right = right;

    return node;
}

int getNumNodes(Node* node)
{
    int numNodes = 0;

    if(node->left != nullptr)
    {
        numNodes++;
    }

    if(node->right != nullptr)
    {
        numNodes++;
    }

    return numNodes;
}

struct comparisonNode
{
    bool operator()(Node* l, Node* r)
    {
        if(l->num != r->num)
        {
            return l->num > r->num;
        }
        else if(getNumNodes(l) != getNumNodes(r))
        {
            return getNumNodes(l) > getNumNodes(r);
        }
        else
        {
            return l->letter > r->letter;
        }

    }
};

struct comparisonString
{
    bool operator()(string a, string b)
    {
        return a.size() > b.size();
    }
};




void encode(Node* root, string encoding, map<char, string>& encodings)
{
    if(root == nullptr)
    {
        return;
    }

    if(root->left == nullptr && root->right == nullptr)
    {
        encodings[root->letter] = encoding;
    }

    encode(root->left, encoding + '0', encodings);
    encode(root->right, encoding + '1', encodings);
}


void decode(Node* huffTreeRoot, int& ndx, string encoded, ostream& outData)
{
    if(huffTreeRoot == nullptr)
    {
        return;
    }

    if(!huffTreeRoot->left && !huffTreeRoot->right)
    {
        outData << huffTreeRoot->letter;
        return;
    }

    ndx++;

    if(encoded[ndx] == '0')
    {
        decode(huffTreeRoot->left, ndx, encoded, outData);
    }
    else
    {
        decode(huffTreeRoot->right, ndx, encoded, outData);
    }
}




int main(int argc, char* argv[])
{
    ifstream inData;
    ofstream outData;

    if(argc < 4)
    {
        cout << "Not enough arguments, try again";
        return 1;
    }

    inData.open(argv[2]);
    outData.open(argv[3]);

    if(!inData)
    {
        cout << "file not opened" << endl;
        return 1;
    }

    char argumentHuff[] = "-huff";
    char argumentUnhuff[] = "-unhuff";
    if(strcmp(argv[1], argumentHuff) == 0)
    {
        priority_queue <Node*, vector<Node*>, comparisonNode> huffTree;
        map<char, string> encodings;
        int charCounts[LETTERCOUNT] = { 0 };
        string temp, encoding;
        map<char, int> frequency; //character, occurences
        char c;
        Node* tempNode;
        ///encoding
        while(inData.get(c))
        {
            frequency[c]++;
        }

        //make the priority queue
        for (auto const& itr : frequency)
        {
            huffTree.push(getNode(itr.first, itr.second, nullptr, nullptr));
        }

        //make the huffman encoding tree
        while(huffTree.size() != 1)
        {
            Node* left = huffTree.top();
            huffTree.pop();
            Node* right = huffTree.top();
            huffTree.pop();

            int total = left->num + right->num;
            huffTree.push(getNode('0', total, left, right));
        }

        Node* root = huffTree.top(); //entire tree in root, now

        encode(root, "", encodings);

        for(auto itr : encodings)
        {
            outData << (int)itr.first << " " << itr.second << " ";
        }
        outData << endl;

        inData.close();
        inData.open(argv[2]);
        //outData.open("myOutput.txt");

        while(!inData.eof())
        {
            inData >> temp;

            for(int ndx = 0; ndx < temp.size(); ndx++)
            {
                outData << encodings[temp[ndx]];
            }
        }

    }
    else if(strcmp(argv[1], argumentUnhuff) == 0)
    {
        ///decoding

        map<char, string> encodings;
        string values;
        int asciiChar;
        string code;
        stringstream ss;
        priority_queue <string, vector<string>, comparisonString> huffTree;


        getline(inData, values);
        ss.str(values);

        while(ss >> asciiChar)
        {
            ss >> code;

            char ndxChar = (char)asciiChar;
            encodings[ndxChar] = code;
            //encodings[root->letter] = encoding;
        }

        Node* huffTreeRoot = getNode('\0', 0, nullptr, nullptr);


        for(auto& itr : encodings)
        {
            Node* tempNode = huffTreeRoot;
            string encoding = itr.second;

            for(int i = 0; i < encoding.length(); i++)
            {
                if(encoding[i] == '0')
                {
                    if(tempNode->left == nullptr)
                    {
                        tempNode->left = getNode('\0', 0, nullptr, nullptr);
                    }


                    if(i == encoding.length() - 1)
                    {
                        tempNode = tempNode->left;
                        tempNode->letter = itr.first;
                    }
                    else
                    {
                        tempNode = tempNode->left;
                    }

                }
                else
                {
                    if(tempNode->right == nullptr)
                    {
                        tempNode->right = getNode('\0', 0, nullptr, nullptr);
                    }


                    if(i == encoding.length() - 1)
                    {
                        tempNode = tempNode->right;
                        tempNode->letter = itr.first;
                    }
                    else
                    {
                        tempNode = tempNode->right;
                    }

                }
            }
        }

        string encodedLine;
        inData >> encodedLine;


        int ndx = -1;
        int len = encodedLine.length() - 2;
        while(ndx < len)
        {
            decode(huffTreeRoot, ndx, encodedLine, outData);
        }



    }
    else
    {
        cout << "Command not recognized, please try again and type -huff or -unhuff as the command" << endl;
    }

    inData.close();
    outData.close();

    return 0;
}

