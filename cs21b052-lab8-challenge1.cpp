#include <iostream>
#include<bits/stdc++.h>

using namespace std;

/* 

I stored my paragraph as a vector of words (using vector data structure).
whenever I do "add_sentence" or "add_word", I append all new words added to paragraph to vector of words. For "delete(int x)" I pop_back() last x words of vector. Before adding words you check for "wordLimitExceededException" ,similarly before deletion you check "NothingToDeleteException".
   
Since undo() and redo() operations follow "Last in First out" principle, that is the last operation performed should be undone first, I used "Stack Data Structure" for undoInstructions and RedoInstructions.
   
   undoInstructions stores pending paragraph operations that has not yet been undone. UndoInstructions.top() gives recent operation that has not yet been done and throws exception whenever this stack is empty.Based on undoInstructions, the words or sentences that need to be added or deleted are stored in undoThings. Everytime you perform undo() you push the instruction that's undone now into redoInstructions. The undoThings into RedoThings.
   
we empty RedoInstructions and RedoThings stack every time we perform "add_sentence" or "add_word" or "deletion". redo() throws exceptions whenever RedoInstructions stack is empty().

Now, for print() operation I followed the ref shared from CS2800. It throws exception if the paragraph is empty(words.size() = 0).

Using "Stack" for undo, redo operations and "Dynamming programming" for print() makes my code optimal.
*/
   
class Paragraph {
    // design your data structure here
    
    
    private:
    
    int numOfWords;                  /*number of words in current sentence*/
    int characterLimit;              /*number of characters per line*/
    int wordLimit;                   /*total number of words in paragraph*/
    
    stack<string> undoThings;                     
    stack<string> undoInstructions; /* pending paragraph operation that has not yet been undone. */
    stack<int> num;                 /* num.top() stores number of words to be added or deleted when we do undo() */               
    
    vector<string> words;           /*words in current paragraph.*/
   
    stack<string> redoInstructions;  /* pending undo() operations */
    stack<string> redoThings;
    
    public:
    
    class UndoNotPossibleException{};
    class RedoNotPossibleException{};
    class NothingToDeleteException{};
    class EmptyParagraphException{};
    class WordLimitExceededException{};
    
    
    Paragraph(int n, int c, int w, string s)
    {
        numOfWords = n;
        characterLimit = c;
        wordLimit = w;
        
        undoThings.push(s);
        num.push(n);         
        stringstream str(s);
        string word;
        while(str>>word)
        {
            words.push_back(word);
        }
        undoInstructions.push("add_sentence");

    }
    
    void print()
    {
        if(words.size() == 0)
        {
            throw EmptyParagraphException();
        }
        else
        {
            int n = words.size();
            
            vector<int> wordsSize(n);    /*wordsSize[i] stores sum of size of words from words[0] to words[i].*/
            
            for(int i = 0 ; i < n ; i++)
            {
                if(i == 0)
                {
                    wordsSize[0] = words[0].size();
                }
                else
                {
                    wordsSize[i] = wordsSize[i-1] + words[i].size();
                }
            }
            
            
            /*b[i][j] - badness of line that starts with words[i] and ends with words[j]*/
            vector<vector<int>>b(n,vector<int>(n)); 
            
            
            for(int i = 0 ; i < n ; i++)
            {
                for(int j = i ; j < n ; j++)
                {
                    int len;   /*length of text starting with words[i] and ending with words[j]*/
                    
                    if(i == 0)
                    {
                        len = wordsSize[j] + (j-i);
                    }
                    else
                    {
                        len = wordsSize[j] - wordsSize[i-1] + (j-i); 
                    }
                    if(len > characterLimit)
                    {
                        b[i][j] = INT_MAX;
                    }
                    else
                    {
                        int a = characterLimit - len;
                        b[i][j] = pow(a , 3);
                    }
                }
            }
            
            vector<int> justify(n+1);     /*justify[i] - badness of paragraph starting with words[i]*/
            
            vector<int> breakIndex(n+1);  /*breakIndex[i] is the index of word where first line of justify[i] splits*/
            
            justify[n] = 0;
            for(int i = 0 ; i < n ; i++)
            {
                justify[i] = INT_MAX;
            }
            for(int i = n-1 ; i >= 0 ; i--)
            {
                int x;
                for(int k = i ; k < n ; k++)
                {
                    if(b[i][k] < INT_MAX)
                    {
                       if(b[i][k] + justify[k+1] < justify[i])
                        {
                            justify[i] = b[i][k] + justify[k+1];
                            x = k+1;
                        }   
                    }
                }
                breakIndex[i] = x;
            }
            
            vector<int> lines; /*lines[i] stores index of end word of line i in the paragraph*/
            
            int start = 0;
            while(start != n)
            {
                lines.push_back(breakIndex[start]-1);
                start = breakIndex[start];
            }
            
            int L = lines.size();
            
            for(int i = 0 ; i < L-1 ; i++)
            {
                int start,end;
                end = lines[i];
                int gap;
                if(i == 0)
                {
                   start = 0; 
                   gap = characterLimit - (wordsSize[end]);
                }
                else
                {
                    start = lines[i-1]+1;
                    gap = characterLimit - (wordsSize[end] - wordsSize[start-1]) ;
                }
                
                if(end-start != 0)
                {
                     int spaces = gap/(end-start);  /* used for centre align */
                    int extra = gap%(end-start);
                    int r = 0;
                    
                    for(int j = start ; j < end ; j++)
                    {
                        cout<<words[j];
                        for(int k = 0 ; k < spaces ; k++)
                        {
                            cout<<" ";
                        }
                        if(r < extra)
                        {
                            cout<<" ";
                            r++;
                        }
                   }
                    cout<<words[end]<<endl;
                }
                else
                {
                   cout<<words[start]<<endl; 
                }
            }
            
            int end = lines[L-1];
            if(L == 1)
            {
                start = 0;
            }
            else
            {
                start = lines[L-2] + 1;
            }
            for(int i = start ; i <= end ; i++)
            {
                cout<<words[i]<<" ";
            }
            cout<<endl;
        }
    }
    
    void add_word(string word)
    {
        if(numOfWords < wordLimit)
        {
            undoThings.push(word);
            num.push(1);
            numOfWords++; 
            words.push_back(word);
            undoInstructions.push("add_word");
            while(!redoInstructions.empty())
            {
                redoInstructions.pop();
                redoThings.pop();
            }
        }
        else
        {
            throw WordLimitExceededException();
        }
    }
    void add_sentence(int n , string s)
    {
        if(numOfWords + n <= wordLimit)
        {
            numOfWords = numOfWords + n;
            undoThings.push(s);
            num.push(n);
            stringstream str(s);
            string word;
            while(str >> word)
            {
                words.push_back(word);
            }
            undoInstructions.push("add_sentence");
            while(!redoInstructions.empty())
            {
                redoInstructions.pop();
                redoThings.pop();
            }
        }
        else
        {
            throw WordLimitExceededException();
        }
    }
    void undo()
    {
        if(undoInstructions.empty())
        {
            throw UndoNotPossibleException();
        }
        else
        {
            
            if(undoInstructions.top() == "add_sentence" || undoInstructions.top() == "add_word")
            {
                int n1 = num.top();   /* number of words to be deleted in current sentence */
                numOfWords = numOfWords - num.top();
                while(n1--)
                {
                    words.pop_back();
                }
               
            }
            else if(undoInstructions.top() == "delete")
            {
                
                numOfWords = numOfWords + num.top();
                
                stringstream str(undoThings.top());
                string word;
                while(str>>word)
                {
                    words.push_back(word);
                }   
            }

            redoInstructions.push(undoInstructions.top()); 
            redoThings.push(undoThings.top());
            undoInstructions.pop();
            undoThings.pop();
            num.pop();
            
        }
    }
    void redo()
    {
        if(redoInstructions.empty())
        {
            throw RedoNotPossibleException();
        }
        else
        {
            
            if(redoInstructions.top() == "add_sentence" || redoInstructions.top() == "add_word")
            {
                stringstream str(redoThings.top());
                string word;
                int n1 = 0;
                while(str>>word)
                {
                    words.push_back(word);
                    n1++;
                }
                numOfWords = numOfWords + n1;
                num.push(n1);
            }
            else if(redoInstructions.top() == "delete")
            {
                stringstream str(redoThings.top());
                string word;
                int n1 = 0;
                while(str>>word)
                {
                    words.pop_back();
                    n1++;
                }
                numOfWords = numOfWords - n1;
                num.push(n1);
            }
            
            undoInstructions.push(redoInstructions.top());
            undoThings.push(redoThings.top());
            redoInstructions.pop();
            redoThings.pop();
            
        }
    }
    void deletion(int x)
    {
        if(words.size() < x)
        {
            throw NothingToDeleteException();
        }
        else
        {
            numOfWords = numOfWords - x;
            num.push(x);
            string s = words[words.size()-1];
            x--;
            words.pop_back();
            while(x--)
            {
                s =  words[words.size()-1] +" "+s;
                words.pop_back();
            }
            undoInstructions.push("delete");
            undoThings.push(s);            
            while(!redoInstructions.empty())
            {
                redoInstructions.pop();
                redoThings.pop();
            }
        }
    }
    
     
    
};

int main() {
      // your driver code here
    string s ; cin>>s;
    int numOfWords , character_limit , wordLimit;
    cin>>numOfWords>>character_limit>>wordLimit;
    
    
    string sentence = "";
    for(int i = 0 ; i < numOfWords ; i++)
    {
        cin>>s;
        sentence = sentence + s+" ";
    }
    Paragraph P(numOfWords,character_limit,wordLimit,sentence);
    
    cin>>sentence;
    

        while(sentence != "end")
        {
            try
            {
                if(sentence == "print")
                {
                    P.print(); 
                }
                else if(sentence == "add_word")
                {
                    string word;
                    cin>>word;
                    P.add_word(word);

                }
                else if(sentence == "add_sentence")
                {
                    int n;
                    cin>>n;
                    string s1 = "";
                    for(int i = 0 ; i < n ; i++)
                    {
                       string temp;
                        cin>>temp;
                        s1 = s1 + temp +" ";
                    }
                    P.add_sentence(n,s1);

                }
                else if(sentence == "undo")
                {
                    P.undo();
                }
                else if(sentence == "redo")
                {
                    P.redo();
                }
                else if(sentence == "delete")
                {
                    int n;
                    cin>>n;
                    P.deletion(n);
                }
            }
            catch(Paragraph::UndoNotPossibleException)
            {
                cout << "UndoNotPossibleExceptionRaised"<<endl;
            }
            catch(Paragraph::RedoNotPossibleException)
            {
                cout << "RedoNotPossibleExceptionRaised"<<endl;
            }
            catch(Paragraph::NothingToDeleteException)
            {
                cout << "NothingToDeleteException"<<endl;
            }
            catch(Paragraph::EmptyParagraphException)
            {
                cout << "EmptyParagraphException"<<endl;
            }
            catch(Paragraph::WordLimitExceededException)
            {
                cout << "WordLimitExceededException"<<endl;
            }
            
            cin>>sentence;
        }   
    
    return 0;
}