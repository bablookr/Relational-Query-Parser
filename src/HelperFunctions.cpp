#include "functions.h"


/*FILE PARSER
Takes the filename as input
Returns the table with the filename */

Table* ParseFile(const char* filename){
    Table* table=new Table();
    vector<vector<string> > temp;
    vector<string> row;
    string attribute,type;

    ifstream file;                                                          // declare the filestream
    file.open(filename);                                                    // open the file
    if(!file.good())
        throw "Invalid filename or operation.";

    string line;
    while(getline(file,line)){                                              // get every line of file
        stringstream ss(line);                                              // declare the stringstream
        string value;
        while(getline(ss,value,',')){                                       // get every word of a line separated by ','
            if(value[0]!='@')
                row.push_back(value);
            else{
                if(value[1]=='r')                                           // get the relation name
                    table->name=value.substr(10,value.length());
                if(value[1]=='a'){                                          // get the attribute details
                    attribute=value.substr(11,value.length()-19);
                    type=value.substr(value.length()-7,value.length());
                    table->att.push_back(make_pair(attribute,type));
                }
            }
        }
        if(!row.empty())
            temp.push_back(row);
        row.clear();
    }
    file.close();

    table->vec=temp;
    return table;
}


/*OPERATOR FUNCTION
Takes the table,operation and the arguments as input
Returns the result table */

Table* Operate(Table* table,string operation,string args){
    if(operation=="SELECT")
        return Select(table,args);
    else if(operation=="PROJECT")
        return Project(table,args);
    else if(operation=="RENAME")
        return Rename(table,args);
    else if(operation=="UNION")
        return Union(table,ParseQuery(args.substr(1,args.length()-2)));
    else if(operation=="SETDIFFERENCE")
        return SetDifference(table,ParseQuery(args.substr(1,args.length()-2)));
    else if(operation=="CARTESIANPRODUCT")
        return CartesianProduct(table,ParseQuery(args.substr(1,args.length()-2)));
    else{
        string error="Invalid operation "+operation;
        throw error.c_str();
    }
}


/*QUERY PARSER
Takes the query as input
Returns the  result table */

Table* ParseQuery(string query){
    if(!bracketsBalanced(query))                                            // return INVALID if brackets are not balanced
        throw "INVALID QUERY.";

    if(query[query.length()-1]==']' || query.find('(')==string::npos){      // handle the corner cases of queries
        query="SELECT("+query+") *";
    }

    stack<string> stk;
    queue<string> q;

    int i=-1,sep,in;
    int l=query.find(')');
    while(sep>=0){                                                          // get operations by splitting with '('
        sep=query.find('(',i+1);
        if(sep>0 && sep<l){
            stk.push(query.substr(i+1,sep-i-1));
            in=sep;
        }
        i=sep;
    }

    int j=query.find(')');
    string relation=query.substr(in+1,j-in-1);                              // get relation name to be operated

    string s;
    int lb,ub;
    sep=0;
    while(sep!=query.rfind(')') && sep>=0){
        sep=query.find(')',j+1);                                            // get args corresponding to different operations
        s=query.substr(j+2,sep-j-2);
        if(s[0]=='['){
            ub=query.find(']',j+2);
            j=sep;
            while(sep<ub){
                sep=query.find(')',j+1);
                s+=query.substr(j,sep-j);
                j=sep;
            }
        }
        q.push(s);
        j=sep;
    }
    if(sep>=0)
        q.push(query.substr(j+2,query.length()-j-2));

    relation="../res/"+relation+".txt";
    Table* temp = ParseFile(relation.c_str());                                // open the table file and parse
    while(!stk.empty() && !q.empty()){                                        // recursively apply all the operations
        temp= Operate(temp,stk.top(),q.front());
        stk.pop(); q.pop();
    }
    return temp;
}


/*INDEX MAP FUNCTION
Takes the table as input
Returns the map of indices of attributes of table */

map<string,int> IndexMap(Table* table){
    map<string,int> m;
    for(int i=0;i<table->att.size();i++)
        m[table->att[i].first]=i+1;
    return m;
}


/*INFIX TO POSTFIX CONVERTER
Takes the infix string as input
Returns the vector containing the args in postfix order */

vector<string> postfix(string args){
    vector<string> pros;
    string st,s;
    int pos;

    int i1=0,j1=0,c1=0;
    for(i1=0;i1<=args.length();i1++){
        if(args[i1]=='|'|| i1==args.length()){                                  // split the string by '|'
            st=args.substr(j1,i1-j1);
            int i2=0,j2=0,c2=0;
            for(i2=0;i2<=st.length();i2++){
                if(st[i2]=='&'|| i2==st.length()){                              // split the split strings by '&
                    s=st.substr(j2,i2-j2);
                    pos=s.find_first_of("<=>");
                    pros.push_back(s.substr(0,pos));                            // split again the attribute name and value
                    if(s[pos+1]=='"')
                        pros.push_back(s.substr(pos+2,s.length()-pos-3));
                    else
                        pros.push_back(s.substr(pos+1,s.length()-pos-1));
                    pros.push_back(string(1,s[pos]));                           // push the arithmetic operator
                    j2=i2+1;
                    c2++;
                    if(c2!=1) pros.push_back(string(1,'&'));                    // push the '&' operator
                }
            }
            j1=i1+1;
            c1++;
            if(c1!=1) pros.push_back(string(1,'|'));                            // push the '|' operator
        }
    }
    return pros;
}


/*CHECK IF CHARACTER IS OPERATOR
Takes the character as input
Returns true if charcter is operator */

bool isOperator(string c){
    if(c==">"||c=="<"||c=="="||c=="&"||c=="|")
        return true;
    return false;
}


/*CHECKS THE TRUTH VALUE OF A STATEMENT
Takes two strings and comparator as input
Returns the truth value of the statement */

bool check(string st,string value,string operation){
    if(operation=="=")
        return st==value;
    else if(operation==">")
        return atoi(st.c_str())>atoi(value.c_str());
    else if(operation=="<")
        return atoi(st.c_str())<atoi(value.c_str());
    else
        throw "INVALID OPERATION "+operation;
}



/*CHECKS IF THE BRACKETS MATCH
Takes two characters as input
Returns true if brackets match */

bool match(char c1,char c2){
    if((c1=='(' && c2==')')||(c1=='{' && c2=='}')||(c1='[' && c2==']'))
        return true;
    return false;
}


/*CHECKS IF BRACKETS ARE BALANCED
Takes a string query as input
Returns true if the brackets are balanced in the query */

bool bracketsBalanced(string query){
    stack<char> stk;
    char c;
    int i=0;
    while(query[i]){
        if(query[i]=='(' || query[i]=='{' || query[i]=='[')                 // push if opening bracket
            stk.push(query[i]);
        if(query[i]==')' || query[i]=='}' || query[i]==']'){                // if closing bracket,
            if(stk.empty())
                return false;
            else{                                                           // check if it matches with the top element
                c=stk.top();
                stk.pop();
                if(!match(c,query[i]))
                    return false;
            }
        }
        i++;
    }
    if(stk.empty())
        return true;
    return false;
}


/*DISPLAY TABLE
Takes the table eas input
Displays the contents of a table */

void Display(Table* table){
    for(int i=0;i<table->att.size();i++)
        cout<<table->att[i].first<<"\t\t";
    cout<<endl;
    for(int i=0;i<table->vec.size();i++){
        for(int j=0;j<table->att.size();j++)
            cout<<table->vec[i][j]<<"\t\t";
        cout<<endl;
    }
    cout<<endl;
}


/*DISPLAYS THE QUERY FORMATS
Void input
Display the query formats on prompt */

void QueryFormats(){
    cout<<"Query Formats\n\n"
            "SELECT(table) condition1^condition2^......\n"
            "PROJECT(table) attribute1,attribute2,.....\n"
            "RENAME(table) new_name\nOR, RENAME(table_name) new_name{new names of all attributes}\n\n"
            "UNION(table1) [table2]\n"
            "SETDIFFERENCE(table1) [table2]\n"
            "CARTESIANPRODUCT(table1) [table2]\n";
    cout<<endl<<endl;
}

