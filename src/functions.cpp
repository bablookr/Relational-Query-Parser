#include "functions.h"

//READS THE FILE AND RETURNS A TABLE
Table* ParseFile(const char* filename){
    Table* table=new Table();
    vector<vector<string> > temp;
    vector<string> row;
    string attribute,type;

    ifstream file;
    file.open(filename);
    if(!file.good())
        throw "Invalid filename or operation.";

    string line;
    while(getline(file,line)){
        stringstream ss(line);
        string value;
        while(getline(ss,value,',')){
            if(value[0]!='@')
                row.push_back(value);
            else{
                if(value[1]=='r')
                    table->name=value.substr(10,value.length());
                if(value[1]=='a'){
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

Table* Select(Table* table,string args){
    if(args=="*")
        return table;

    Table* temp=new Table();
    temp->name=table->name;
    temp->att=table->att;

    vector<vector<string> > ve;
    vector<string> v;
    v=postfix(args);

    map<string,int> m;
    m=IndexMap(table);
    int index;

    stack<string> stk;
    string a,b;

    for(int row=0;row<table->vec.size();row++){
        for(int i=0;i<v.size();i++){
            if(!isOperator(v[i]))
                stk.push(v[i]);
            else{
                a=stk.top();
                stk.pop();
                b=stk.top();
                stk.pop();
                if(v[i]=="&"){
                    if(a=="false" || b=="false")
                        stk.push("false");
                    else
                        stk.push("true");
                }
                else if(v[i]=="|"){
                    if(a=="true" || b=="true")
                        stk.push("true");
                    else
                        stk.push("false");
                }
                else{
                    index=m[b]-1;
                    if(index==-1)
                        throw "Invalid attribute name, in SELECT operation";
                    if(check(table->vec[row][index],a,v[i]))
                        stk.push("true");
                    else
                        stk.push("false");
                }
            }
        }
        if(stk.top()=="true")
            ve.push_back(table->vec[row]);

        stk.pop();
    }

    temp->vec=ve;
    return temp;
}

Table* Project(Table* table,string args){
    Table* temp=new Table();
    temp->name=table->name;

    vector<pair<string,string> > a;
    vector<string> pros;
    int i=0,j=0;
    for(i=0;i<=args.length();i++){
        if(args[i]==','|| i==args.length()){
            pros.push_back(args.substr(j,i-j));
            j=i+1;
        }
    }

    vector<int> index;
    for(j=0;j<pros.size();j++){
        for(i=0;i<table->att.size();i++){
            if(table->att[i].first==pros[j]){
                a.push_back(table->att[i]);
                index.push_back(i);
                break;
            }
        }
    }

    if(a.size()!=pros.size())
        throw "Attribute error, in PROJECT operation.";

    for(int i=0;i<pros.size();i++){
        for(int j=i+1;j<pros.size();j++){
            if(pros[i]==pros[j]){
                throw "Attribute repeated, in PROJECTION operation.";
                break;
            }
        }
    }

    set<vector<string> > proset;
    set<vector<string> > :: iterator itr;
    for(int i=0;i<table->vec.size();i++){
        vector<string> rec;
        for(int j=0;j<index.size();j++)
            rec.push_back(table->vec[i][index[j]]);
        proset.insert(rec);
    }

    vector<vector<string> > v;
    for(itr=proset.begin();itr!=proset.end();itr++)
        v.push_back(*itr);

    temp->vec=v;
    temp->att=a;
    return temp;
}

Table* Rename(Table* table,string args){
    string str,table_name;
    int sep1,sep2;
    sep1=args.find('{');
    sep2=args.find('}');
    table_name=args.substr(0,sep1);


    Table* temp=new Table();
    temp->name=table_name;
    temp->vec=table->vec;
    temp->att=table->att;

    if(sep1!=string::npos && sep2!=string::npos){
        str=args.substr(sep1+1,sep2-sep1-1);
        vector<string> names;
        int i=0,j=0;
        for(i=0;i<=str.length();i++){
            if(str[i]==','|| i==str.length()){
                if(str.substr(j,i-j)==" ")
                    throw "Invalid name";
                names.push_back(str.substr(j,i-j));
                j=i+1;
            }
        }
        if(names.size()!=table->att.size())
            throw "Name error, in RENAME operation.";

        for(int i=0;i<names.size();i++){
            for(int j=i+1;j<names.size();j++){
                if(names[i]==names[j]){
                    throw "Names repeated, in RENAME operation.";
                    break;
                }
            }
        }

        for(int k=0;k<temp->att.size();k++)
            temp->att[k].first=names[k];
    }
    return temp;
}

Table* Union(Table* table1, Table* table2){
    if(table1->att!=table2->att)
        throw "Attribute error, in UNION operation.";

    Table* temp=new Table();
    temp->name="Union";
    temp->att=table1->att;

    set<vector<string> > proset;
    set<vector<string> > :: iterator itr;
    for(int i=0;i<table1->vec.size();i++){
        vector<string> rec;
        for(int j=0;j<table1->att.size();j++)
            rec.push_back(table1->vec[i][j]);
        proset.insert(rec);
    }
    for(int i=0;i<table2->vec.size();i++){
        vector<string> rec;
        for(int j=0;j<table2->att.size();j++)
            rec.push_back(table2->vec[i][j]);
        proset.insert(rec);
    }

    vector<vector<string> > v;
    for(itr=proset.begin();itr!=proset.end();itr++)
        v.push_back(*itr);

    temp->vec=v;
    return temp;
}

Table* SetDifference(Table* table1,Table* table2){
    if(table1->att!=table2->att)
        throw "Attribute error, in SETDIFFERENCE operation.";

    Table* temp=new Table();
    temp->name="SetDifference";
    temp->att=table1->att;

    vector<vector<string> > v;
    int k;
    for(int i=0;i<table1->vec.size();i++){
        k=0;
        for(int j=0;j<table2->vec.size();j++){
            if(table1->vec[i]==table2->vec[j])
                k++;
        }
        if(k==0) v.push_back(table1->vec[i]);
    }
    temp->vec=v;
    return temp;
}

Table* CartesianProduct(Table* table1,Table* table2){
    Table* temp=new Table();
    temp->name="CartesianProduct";
    temp->att=table1->att;

    for(int i=0;i<table2->att.size();i++)
        temp->att.push_back(table2->att[i]);

    for(int i=0;i<table1->att.size();i++){
        for(int j=0;j<table2->att.size();j++){
            if(table1->att[i].first==table2->att[j].first){
                throw "Attribute names repeated, in CARTESIANPRODUCT operation.";
                break;
            }
        }
    }

    vector<vector<string> > v;
    vector<string> u;
    for(int i=0;i<table1->vec.size();i++){
        for(int j=0;j<table2->vec.size();j++){
            for(int k=0;k<table1->att.size();k++)
                u.push_back(table1->vec[i][k]);
            for(int k=0;k<table2->att.size();k++)
                u.push_back(table2->vec[j][k]);
            v.push_back(u);
            u.clear();
        }
    }
    temp->vec=v;
    return temp;
}


//OPERATES ON THE TABLE WITH GIVEN ARGUMENTS AND RETURNS A TABLE
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

//RETURNS THE TABLE AFTER PARSING THE INPUT QUERY
Table* ParseQuery(string query){
    if(!bracketsBalanced(query))
        throw "INVALID QUERY.";

    if(query[query.length()-1]==']' || query.find('(')==string::npos){
        query="SELECT("+query+") *";
    }

    stack<string> stk;
    queue<string> q;

    int i=-1,sep,in;
    int l=query.find(')');
    while(sep>=0){
        sep=query.find('(',i+1);
        if(sep>0 && sep<l){
            stk.push(query.substr(i+1,sep-i-1));
            in=sep;
        }
        i=sep;
    }

    int j=query.find(')');
    string relation=query.substr(in+1,j-in-1);

    string s;
    int lb,ub;
    sep=0;
    while(sep!=query.rfind(')') && sep>=0){
        sep=query.find(')',j+1);
        s=query.substr(j+2,sep-j-2);
        if(s[0]=='['){
            ub=query.find(']',j+2);
            j=sep;
            while(sep<ub){
                sep=query.find(')',j+1);
                s+=query.substr(j,sep-j);
                j=sep;
            }
            //cout<<s<<"\t\t\t"<<j+2<<"\t\t\t"<<sep<<endl;
        }
        //cout<<s<<endl;
        q.push(s);
        j=sep;
    }
    if(sep>=0)
        q.push(query.substr(j+2,query.length()-j-2));

    relation="../res/"+relation+".txt";
    Table* temp = ParseFile(relation.c_str());
    while(!stk.empty() && !q.empty()){
        temp= Operate(temp,stk.top(),q.front());
        stk.pop(); q.pop();
    }
    return temp;
}

//HELPER FUNCTION TO CHECK IF A CHAR IS AN OPERATOR
bool isOperator(string c){
    if(c==">"||c=="<"||c=="="||c=="&"||c=="|")
        return true;
    return false;
}

//RETURNS AN INDEX-MAP FOR THE ATTRIBUTES OF A TABLE
map<string,int> IndexMap(Table* table){
    map<string,int> m;
    for(int i=0;i<table->att.size();i++)
        m[table->att[i].first]=i+1;
    return m;
}

//HELPER FUNCTION TO GET THE TRUTH VALUE OF A STATEMENT
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

//HELPER FUNCTION TO CONVERT INFIX ARGS TO POSTFIX
vector<string> postfix(string args){
    vector<string> pros;
    string st,s;
    int pos;

    int i1=0,j1=0,c1=0;
    for(i1=0;i1<=args.length();i1++){
        if(args[i1]=='|'|| i1==args.length()){
            st=args.substr(j1,i1-j1);
            //pros.push_back(st);
            int i2=0,j2=0,c2=0;
            for(i2=0;i2<=st.length();i2++){
                if(st[i2]=='&'|| i2==st.length()){
                    s=st.substr(j2,i2-j2);
                    //pros.push_back(s);
                    pos=s.find_first_of("<=>");
                    pros.push_back(s.substr(0,pos));
                    if(s[pos+1]=='"')
                        pros.push_back(s.substr(pos+2,s.length()-pos-3));
                    else
                        pros.push_back(s.substr(pos+1,s.length()-pos-1));
                    pros.push_back(string(1,s[pos]));
                    j2=i2+1;
                    c2++;
                    if(c2!=1) pros.push_back(string(1,'&'));
                }
            }
            j1=i1+1;
            c1++;
            if(c1!=1) pros.push_back(string(1,'|'));
        }
    }
    return pros;
}

//HELPER FUNCTION TO CHECK MATCHING BRACKETS
bool match(char c1,char c2){
    if((c1=='(' && c2==')')||(c1=='{' && c2=='}')||(c1='[' && c2==']'))
        return true;
    return false;
}

//HELPER FUNCTION TO CHECK BALANCED BRACKETS
bool bracketsBalanced(string query){
    stack<char> stk;
    char c;
    int i=0;
    while(query[i]){
        if(query[i]=='(' || query[i]=='{' || query[i]=='[')
            stk.push(query[i]);
        if(query[i]==')' || query[i]=='}' || query[i]==']'){
            if(stk.empty())
                return false;
            else{
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

//DISPLAYS THE INPUT TABLE
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
