#include "functions.h"


/*SELECT FUNCTION FOR A RELATION
Takes the table and args as input
Returns the result table */

Table* Select(Table* table,string args){
    if(args=="*")                                                       // if the whole table is to be selected
        return table;

    Table* temp=new Table();
    temp->name=table->name;
    temp->att=table->att;

    vector<vector<string> > ve;
    vector<string> v;
    v=postfix(args);                                                    // convert the infix args to postfix

    map<string,int> m;
    m=IndexMap(table);                                                  // get the index-map of the table
    int index;

    stack<string> stk;
    string a,b;

    for(int row=0;row<table->vec.size();row++){                         // for every tuple of table
        for(int i=0;i<v.size();i++){                                    // evaluate the postfix
            if(!isOperator(v[i]))                                       // push if not operator
                stk.push(v[i]);
            else{
                a=stk.top();                                            // if operator, pop the top two elemnts
                stk.pop();
                b=stk.top();
                stk.pop();
                if(v[i]=="&"){                                          // get the truth value regarding two elements and operator
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
                else{                                                   // get the truth value regarding a statement
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
        if(stk.top()=="true")                                           // push the tuple if it evaluates to true
            ve.push_back(table->vec[row]);

        stk.pop();
    }

    temp->vec=ve;
    return temp;
}


/*PROJECT FUNCTION FOR A RELATION
Takes the table and args as input
Returns the result table */

Table* Project(Table* table,string args){
    Table* temp=new Table();
    temp->name=table->name;

    vector<pair<string,string> > a;
    vector<string> pros;
    int i=0,j=0;                                                                // get the names of columns to be projected
    for(i=0;i<=args.length();i++){
        if(args[i]==','|| i==args.length()){
            pros.push_back(args.substr(j,i-j));
            j=i+1;
        }
    }

    vector<int> index;
    for(j=0;j<pros.size();j++){                                                 // set the attributes of new table
        for(i=0;i<table->att.size();i++){
            if(table->att[i].first==pros[j]){
                a.push_back(table->att[i]);
                index.push_back(i);
                break;
            }
        }
    }

    if(a.size()!=pros.size())                                                   // check if the attributes to be projected are in table
        throw "Attribute error, in PROJECT operation.";

    for(int i=0;i<pros.size();i++){                                             // check if attributes are not repeated in args
        for(int j=i+1;j<pros.size();j++){
            if(pros[i]==pros[j]){
                throw "Attribute repeated, in PROJECTION operation.";
                break;
            }
        }
    }

    set<vector<string> > proset;
    set<vector<string> > :: iterator itr;
    for(int i=0;i<table->vec.size();i++){                                       // push all vectors into a set
        vector<string> rec;
        for(int j=0;j<index.size();j++)
            rec.push_back(table->vec[i][index[j]]);
        proset.insert(rec);
    }

    vector<vector<string> > v;
    for(itr=proset.begin();itr!=proset.end();itr++)                             // push all vectors in set to vector of vectors
        v.push_back(*itr);

    temp->vec=v;
    temp->att=a;
    return temp;
}


/*RENAME FUNCTION FOR A RELATION
Takes the table and args as input
Returns the result table */

Table* Rename(Table* table,string args){
    string str,table_name;
    int sep1,sep2;
    sep1=args.find('{');                                                            // check if the attributes names are also to be changed
    sep2=args.find('}');
    table_name=args.substr(0,sep1);


    Table* temp=new Table();
    temp->name=table_name;                                                          // set the elements of the new table
    temp->vec=table->vec;
    temp->att=table->att;

    if(sep1!=string::npos && sep2!=string::npos){                                   // if attributes names are also to be changed
        str=args.substr(sep1+1,sep2-sep1-1);
        vector<string> names;
        int i=0,j=0;
        for(i=0;i<=str.length();i++){                                               // get the new names of the respective attributes
            if(str[i]==','|| i==str.length()){
                if(str.substr(j,i-j)==" ")
                    throw "Invalid name";
                names.push_back(str.substr(j,i-j));
                j=i+1;
            }
        }
        if(names.size()!=table->att.size())                                         // check if the names for all attributes are given
            throw "Name error, in RENAME operation.";

        for(int i=0;i<names.size();i++){                                            // check if new names are not repeated
            for(int j=i+1;j<names.size();j++){
                if(names[i]==names[j]){
                    throw "Names repeated, in RENAME operation.";
                    break;
                }
            }
        }

        for(int k=0;k<temp->att.size();k++)                                         // change the names of the attributes
            temp->att[k].first=names[k];
    }
    return temp;
}


/*UNION FUNCTION FOR A RELATION
Takes the table and args as input
Returns the result table */

Table* Union(Table* table1, Table* table2){
    if(table1->att!=table2->att)                                                    // check if attributes of both tables are same
        throw "Attribute error, in UNION operation.";

    Table* temp=new Table();
    temp->name="Union";
    temp->att=table1->att;                                                          //vector of attributes will remain same

    set<vector<string> > proset;
    set<vector<string> > :: iterator itr;
    for(int i=0;i<table1->vec.size();i++){                                          // push all tuples of table1 to a set
        vector<string> rec;
        for(int j=0;j<table1->att.size();j++)
            rec.push_back(table1->vec[i][j]);
        proset.insert(rec);
    }
    for(int i=0;i<table2->vec.size();i++){                                          // push all tuples of table2 to a set
        vector<string> rec;
        for(int j=0;j<table2->att.size();j++)
            rec.push_back(table2->vec[i][j]);
        proset.insert(rec);
    }

    vector<vector<string> > v;                                                      // push all elements of set to the vector
    for(itr=proset.begin();itr!=proset.end();itr++)
        v.push_back(*itr);

    temp->vec=v;
    return temp;
}


/*SETDIFFERENCE FUNCTION FOR A RELATION
Takes the table and args as input
Returns the result table */

Table* SetDifference(Table* table1,Table* table2){
    if(table1->att!=table2->att)                                                    // check if attributes of both tables are same
        throw "Attribute error, in SETDIFFERENCE operation.";

    Table* temp=new Table();
    temp->name="SetDifference";
    temp->att=table1->att;                                                          //vector of attributes will remain same

    vector<vector<string> > v;
    int k;
    for(int i=0;i<table1->vec.size();i++){                                          // set difference of two sets
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


/*CARTESIANPRODUCT FUNCTION FOR A RELATION
Takes the table and args as input
Returns the result table */

Table* CartesianProduct(Table* table1,Table* table2){
    Table* temp=new Table();
    temp->name="CartesianProduct";
    temp->att=table1->att;                                                          // initialize the attributes of temp as of table1

    for(int i=0;i<table2->att.size();i++)                                           // push attributes of table2
        temp->att.push_back(table2->att[i]);

    for(int i=0;i<table1->att.size();i++){                                          // check if attributes names are repeated
        for(int j=0;j<table2->att.size();j++){
            if(table1->att[i].first==table2->att[j].first){
                throw "Attribute names repeated, in CARTESIANPRODUCT operation.";
                break;
            }
        }
    }

    vector<vector<string> > v;
    vector<string> u;
    for(int i=0;i<table1->vec.size();i++){                                          // cross product of two sets
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

