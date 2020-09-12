/*
        Author: Hayden Gemeinhardt
        Since: 7/6/2020
        trie.cpp
        CSC 3102 Project #3 (Honors)
        Instructor: Dr. Shah

        Like last program, my narrative is distributed throughout
        comments in the code. I describe each function in a comment above.
*/

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

/*******************************************************************************/

typedef struct Node {
	int freq, maxfreq;
	string lead, label, meaning;
	bool isWord;
	struct Node* rightSibling, *firstChild;
} Node;

/*******************************************************************************/

/* Follows the outline given in the instructions. Takes firstChild and a char
 * c as an input and searches for the correct node by following rightSibling
 * pointers until it finds the node with c as its leading character. */

Node* findChild(Node* p, char c)
{
	Node* firstChild = new Node;
	if (p->firstChild)
		firstChild = p->firstChild;
	else
		return NULL;
	while ((firstChild->label[0] != c) && (firstChild->rightSibling != NULL))
		firstChild = firstChild->rightSibling;
	if (firstChild->label[0] == c)
		return firstChild;
	else
		return NULL;
}

/*******************************************************************************/

/* No longer needed. */
//string fetchMeaning(string word)
//{
//	string definition = "";
//	int k = word.length();
//	ifstream dict("dict_meaning.txt");
//	while (getline(dict,definition))
//	{
//		string temp = definition.substr(0, k);
//		if (word == definition.substr(0, k))
//		{
//			cout << "found!";
//			return definition;
//		}
//	}
//	return "empty";
//}

/*******************************************************************************/

/* Goes to the right sibling of first child until the first letter of the prefix
 * matches. Then it takes into account how long the label of the node is.
 * Continues through loop until target is found.*/

Node* search(Node* p, string prefix)
{
	Node* temp = p;
	for (int i = 0; i < prefix.length(); i++)
	{
		temp = temp->firstChild;
		while (temp->label[0] != prefix[i])
			temp = temp->rightSibling;
		int k = 1;
		while (temp->label.length() > k && temp->label[i + 1] == prefix[i + 1])
		{
			k++;
			i++;
		}
	}
	return temp;
}

/*******************************************************************************/

/* Code taken from instructions. Will visit every path from the parent node sent
 * from prefixQuery, printing each word when it reaches a node with word=true.*/

void preorder(Node* p, string prefix)
{
	if (p == NULL)
		return;
	prefix = prefix + p->label;
	if (p->isWord)
		cout << prefix << " ";
	Node* q = p->firstChild;
	while (q != NULL)
	{
		preorder(q, prefix);
		q = q->rightSibling;
	}
}

/*******************************************************************************/

/* First finds the node containing the target prefix using the search function.
 * Then, it will call preorder for every child the node has. */

void prefixQuery(Node* p, string prefix)
{
	Node* temp = p;
	temp = search(temp, prefix);
	temp = temp->firstChild;
	while (temp != NULL)
	{
		preorder(temp, prefix);
		temp = temp->rightSibling;
	}
}

/*******************************************************************************/

/* Used for autocomplete. Finds the word containing max frequency. It will
 * continue down the siblings until it finds the subtree with the desired
 * maxfrequency word, then set that child to parent and recursively go until
 * it reaches a node where its frequency equals the desires max frequency. 
 * This skips subtrees that do not contain the word for efficiency. */

string findMaxFreq(Node* p, string prefix, int maxfreq)
{
	Node* temp = p->firstChild;
	while (temp->maxfreq != maxfreq)
	{
		temp = temp->rightSibling;
	}
	prefix = prefix + temp->label;
	if (temp->isWord == true && temp->freq == maxfreq)
		return prefix;
	if (temp->firstChild != NULL)
		prefix = findMaxFreq(temp, prefix, maxfreq);
	return prefix;
}

/*******************************************************************************/

/* For honors option. First finds the node for the prefix entered. Then it
 * calls the findMaxFreq function to find the word with the max frequency
 * of the subtree of the node with the target prefix. */

void autoComplete(Node* p, string prefix)
{
	Node* temp = search(p, prefix);
	if (temp == NULL)
		return;
	int freq = temp->maxfreq;
	string word = findMaxFreq(temp, prefix, temp->maxfreq);
	cout << word;
}

/*******************************************************************************/

/* Taken from instructions. Three different cases for inserting: node already
 * exists for the word, node does not exist and a new child is needed, or the
 * node needs to be split to have both the old and new word for that node. I
 * modified it to insert lexicographically, so the words TRUE and FALSE will
 * be at the beginning since they are capitalized, and the rest of the words
 * will be in alphabetical order a-z afterward. */

void insert(Node* p, string s, string meaning, int freq)
{
	int i = 0, tempfreq = p->maxfreq;

	if (p->maxfreq < freq)
		p->maxfreq = freq;
	while (i < p->label.length() && i < s.length() && p->label[i] == s[i])
		i++;

	s = s.substr(i, s.length());  //s is now unmatched part possibly empty

	if ((s.length() == 0) && (p->label.length() == i))
	{
		p->isWord = true;
		p->freq = freq;
		p->meaning = meaning;
	}
	else if (i == p->label.length()) //p's label exhausted
	{
		Node* q = findChild(p, s[0]);
		if (q == NULL) //make a new child of p
		{
			Node* temp, *prev = new Node;
			Node* r = new Node;
			r->label = s;
			r->isWord = true;
			r->freq = freq;
			r->maxfreq = freq;
			r->meaning = meaning;
			r->firstChild = NULL;

			temp = p->firstChild;
			if (temp != NULL && temp->rightSibling != NULL) //trie has more than two items
			{
				while (temp->rightSibling != NULL && s > temp->label)
				{
					prev = temp;
					temp = temp->rightSibling;
				}

				if (temp == p->firstChild) //if r needs to go first
				{
					p->firstChild = r;
					r->rightSibling = temp;
				}
				else if (s > temp->label && temp->rightSibling == NULL) //if r needs to go last
				{
					r->rightSibling = NULL;
					temp->rightSibling = r;
				}
				else
				{
					r->rightSibling = temp;
					prev->rightSibling = r;
				}
			}
			else
			{
				if (temp == NULL) //if trie is empty
				{
					p->firstChild = r;
					r->rightSibling = NULL;
				}
				else //one item in trie
				{
					if (s > temp->label) //new node needs to be second
					{
						r->rightSibling = NULL;
						temp->rightSibling = r;
					}
					else //new node needs to be first
					{
						p->firstChild = r;
						r->rightSibling = temp;
					}
				}
			}
		} //modified to insert lexicographically
		else //continue inserting recursively
			insert(q, s, meaning, freq);
	}
	else // p->label is not completely consumed then node p will be split
	{
 		string s1 = p->label.substr(i, p->label.length()); // later part of p's label
		string s2 = p->label.substr(0, i);  // earlier part of p's label
		Node* q1 = new Node;
		q1->label = s1;
		Node* q2 = new Node; //s is now original s sans its prefix already matched prefix s2
		q2->label = s;
		q1->firstChild = p->firstChild;  //q1 inherits everything from p
		q1->isWord = p->isWord; 
		q1->freq = p->freq;
		q1->maxfreq = tempfreq;  // this is the original max freq of p
		q1->meaning = p->meaning;
		q1->rightSibling = NULL;
		if (s.length() != 0) 
			p->isWord = false; 
		else 
			p->isWord = true;
		p->firstChild = q2;
		p->label = s2;
		q2->rightSibling = q1;
		q2->isWord = true; 
		q2->freq = freq; 
		q2->maxfreq = freq; 
		q2->meaning = meaning;
		q2->firstChild = NULL;
	}
}

/*******************************************************************************/

/* First creates the root. Then it will continuously insert words from the
 * text document. Below that I implemented an example of the prefixQuery and
 * autoComplete functions. */

void main()
{
	ifstream words("minidict_freq.txt");

	Node* root = new Node;
	root->lead = "";
	root->label = "";
	root->isWord = false;
	root->rightSibling = root->firstChild = NULL;

	string word;
	int freq;

	while (!(words.eof()))
	{
		words >> word >> freq;
		//string meaning = fetchMeaning(word);
		string meaning = ""; //since we are leaving meaning blank now
		insert(root, word, meaning, freq);
	}

	string prefix = "";
	cout << "Enter a prefix to find all words with the given prefix: ";
	cin >> prefix;
//	string prefix = "t"; //for example
	prefixQuery(root, prefix);
	cout << endl << endl << "The word with the most frequent use for autocomplete is: ";
	autoComplete(root, prefix);
}

/*******************************************************************************/