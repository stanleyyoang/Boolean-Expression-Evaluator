//Boolean Expression Evaluator Project
//By: Stanley Yoang

#include <iostream>
#include <string>
#include <vector>

using namespace std;

typedef struct AST *pNODE;
struct AST {
    string info;
    pNODE children[2];
};

struct tokRslt {
    bool success;
    vector<string> syms;
};

struct parseRslt {
    bool success;
    AST ast;
};

struct TPERslt {
    bool val;
    string msg;
};

//Function declarations
void prinTree(AST T);
pNODE cons(string s, pNODE c1, pNODE c2);
tokRslt tokenize(string s);
parseRslt parse(vector<string> V);
pNODE checkConstant(vector<string> V, int index);
pNODE checkUnbreakable(vector<string> V, int start, int stop);
pNODE checkNegation(vector<string> V, int start, int stop);
pNODE checkConjunction(vector<string> V, int start, int stop);
pNODE checkDisjunction(vector<string> V, int start, int stop);
pNODE checkImplication(vector<string> V, int start, int stop);
pNODE checkBooleanExpression(vector<string> V, int start, int stop);
//pNODE checkParen(vector<string> V, int start, int stop);
bool eval(AST T);
TPERslt TPE(string s);
string TPEOut(string s);

void prinTree(AST T) {
    if (T.children[0] == NULL) {
        cout << T.info;
        return;
    }
    
    cout << "(" << T.info << " ";
    prinTree(*(T.children[0]));
    cout << " ";
    
    if (T.children[1] != NULL) {
        prinTree(*(T.children[1]));
    }
    
    cout << ")";
}

pNODE cons(string s, pNODE c1, pNODE c2) {
    pNODE ret = new AST;
    ret->info = s;  // same as (*ret).info = s
    ret->children[0] = c1;
    ret->children[1] = c2;
    return ret;
}

//Function that checks to make sure all characters entered by the user are appropriate for the program to use
//If they are appropriate then adds the characters to the vector string
tokRslt tokenize(string s)
{
    tokRslt tokenize;
    
    //Looks for correct characters, if not appropriate set to false
    for (size_t i = 0; i < s.length(); i++)
    {
        if (s[i] == 'T' || s[i] == 'F' || s[i] == '^' || s[i] == 'v' || s[i] == '~' || s[i] == '<' || s[i] == '=' || s[i] == '>' ||
            s[i] == '(' || s[i] == ')' || s[i] == ' ')
        {
            tokenize.success = true;
            continue;
        }
        else
        {
            tokenize.success = false;
            break;
        }
    }
    
    if (tokenize.success != false)
    {
        tokenize.success = true;
        
        //adds the characters to the vector string
        for (size_t i = 0; i < s.length(); i++)
        {
            //ignore white space
            if (s[i] == ' ')
                continue;
            
            //add => as one string
            else if (s[i] == '=')
            {
                if (s[i + 1] == '>')
                {
                    tokenize.syms.push_back(s.substr(i, 2));
                }
                else
                {
                    tokenize.success = false;
                    break;
                }
                i++;
            }
            
            //add <=> as one string
            else if (s[i] == '<')
            {
                if (s[i + 1] == '=')
                {
                    if (s[i + 2] == '>')
                    {
                        tokenize.syms.push_back(s.substr(i, 3));
                    }
                    else
                    {
                        tokenize.success = false;
                        break;
                    }
                }
                else
                {
                    tokenize.success = false;
                    break;
                }
                i = i + 2;
            }
            
            //add the other symbols one by one
            else
            {
                tokenize.syms.push_back(s.substr(i, 1));
            }
        }
    }
    
    //if symbols are inappropriate, clear the vector
    if (tokenize.success == false)
        tokenize.syms.clear();
    
    return tokenize;
}

//Function used to put the vector string into an AST
//Also ensures that symbols are put in a correct order
parseRslt parse(vector<string> V)
{
	parseRslt expression;
	pNODE expression2;
	int open = 0;
	int close = 0;

	//if vector is empty set AST to null
	if (V.size() == 0)
	{
		expression.success = false;
		expression.ast = *cons("EMPTY", NULL, NULL);
		return expression;
	}

	//ensures that every operand is followed by an operator
	for (size_t i = 0; i < V.size() - 1; i++)
	{
		if (V[i] == "T" || V[i] == "F")
		{
			if (V[i + 1] == "v" || V[i + 1] == "^" || V[i + 1] == "=>" || V[i + 1] == "<=>" || V[i + 1] == ")")
				continue;
			else
			{
				expression.success = false;
				expression.ast = *cons("EMPTY", NULL, NULL);
				return expression;
			}
		}
	}

	if (V[0] == "^" || V[0] == "v" || V[0] == "=>" || V[0] == "<=>")
	{
		expression.success = false;
		expression.ast = *cons("EMPTY", NULL, NULL);
		return expression;
	}

	//ensures that there is an equal amount of open and closed parentheses
	for (size_t i = 0; i < V.size(); i++)
	{
		if (V[i] == "(")
			open++;
		if (V[i] == ")")
			close++;
	}

	if (open != close)
	{
		expression.success = false;
		expression.ast = *cons("EMPTY", NULL, NULL);
		return expression;
	}

	//create the AST
	expression2 = checkBooleanExpression(V, 0, V.size());

	if (expression2 == NULL)
		expression.success = false;
	else
	{
		expression.success = true;
		expression.ast = *expression2;
	}

    if (expression.success == true)
    {
        prinTree(expression.ast);
        return expression;
    }
    else
    {
        return expression;
    }
}

pNODE checkConstant(vector<string> V, int start, int stop)
{
	pNODE expression;

	if (start != stop - 1 || start >= V.size())
	{
		return NULL;
	}

	if (V[start] == "T" || V[start] == "F")
	{
		return cons(V[start], NULL, NULL);
	}
	else
	{
		return cons("EMPTY", NULL, NULL);
	}
}

pNODE checkUnbreakable(vector<string> V, int start, int stop)
{
	pNODE expression;

	if (V[start] == "(" && V[stop - 1] == ")")
	{
		expression = checkBooleanExpression(V, start + 1, stop - 1);
		if (expression != NULL)
			return expression;
	}

	expression = checkConstant(V, start, stop);
	if (expression != NULL)
		return expression;
	else
		return NULL;
}

pNODE checkNegation(vector<string> V, int start, int stop)
{
	pNODE expression;

	for (int i = start; i < stop; i++)
	{
		if (V[i] == "~")
		{
			expression = checkNegation(V, start + 1, stop);
			if (expression != NULL)
				return cons(V[i], expression, NULL);
		}
	}

	expression = checkUnbreakable(V, start, stop);
	if (expression != NULL)
		return expression;
	else
		return NULL;
}

pNODE checkConjunction(vector<string> V, int start, int stop)
{
	pNODE expression1, expression2;

	for (int i = start + 1; i < stop; i++)
	{
		if (V[i] == "^")
		{
			expression1 = checkConjunction(V, start, i);
			expression2 = checkNegation(V, i + 1, stop);
			if (expression1 != NULL && expression2 != NULL)
				return cons(V[i], expression1, expression2);
		}
	}

	expression1 = checkNegation(V, start, stop);

	if (expression1 != NULL)
		return expression1;
	else
		return NULL;
}

pNODE checkDisjunction(vector<string> V, int start, int stop)
{
	pNODE expression1, expression2;

	for (int i = start + 1; i < stop; i++)
	{
		if (V[i] == "v")
		{
			expression1 = checkDisjunction(V, start, i);
			expression2 = checkConjunction(V, i + 1, stop);
			if (expression1 != NULL && expression2 != NULL)
				return cons(V[i], expression1, expression2);
		}
	}

	expression1 = checkConjunction(V, start, stop);

	if (expression1 != NULL)
		return expression1;
	else
		return NULL;
}

pNODE checkImplication(vector<string> V, int start, int stop)
{
	pNODE expression1, expression2;

	for (int i = start + 1; i < stop; i++)
	{
		if (V[i] == "=>")
		{
			expression1 = checkDisjunction(V, start, i);
			expression2 = checkImplication(V, i + 1, stop);
			if (expression1 != NULL && expression2 != NULL)
				return cons(V[i], expression1, expression2);
		}
	}

	expression1 = checkDisjunction(V, start, stop);

	if (expression1 != NULL)
		return expression1;
	else
		return NULL;
}

pNODE checkBooleanExpression(vector<string> V, int start, int stop)
{
	pNODE expression1, expression2;

	for (int i = start + 1; i < stop; i++)
	{
		if (V[i] == "<=>")
		{
			expression1 = checkImplication(V, start, i);
			expression2 = checkBooleanExpression(V, i + 1, stop);
			if (expression1 != NULL && expression2 != NULL)
				return cons(V[i], expression1, expression2);
		}
	}

	expression1 = checkImplication(V, start, stop);

	if (expression1 != NULL)
		return expression1;
	else
		return NULL;
}

//Function used to evaluate the boolean expression
bool eval(AST T)
{
    bool left, right;
    
    //if AST doesn't have any children, it must be a T or F value
    if (T.children[0] == NULL)
    {
        if (T.info == "T")
            return true;
        else if (T.info == "F")
            return false;
        else
            return NULL;
    }
    
    //if AST.info is "~" and only has one child invert the value
    else if (T.info == "~" && T.children[1] == NULL)
    {
        return !eval(*T.children[0]);
    }
    
    //follow the standard semantics of boolean expressions
    else
    {
        //AND statements
        if (T.info == "^")
        {
            left = eval(*T.children[0]);
            right = eval(*T.children[1]);
            
            if (left == true && right == true)
                return true;
            else
                return false;
        }
        
        //OR statements
        else if (T.info == "v")
        {
            left = eval(*T.children[0]);
            right = eval(*T.children[1]);
            
            if (left == false && right == false)
                return false;
            else
                return true;
        }
        
        //IMPLIES statements
        else if (T.info == "=>")
        {
            left = eval(*T.children[0]);
            right = eval(*T.children[1]);
            
            if (left == true && right == false)
                return false;
            else
                return true;
        }
        
        //IFF statements
        else if (T.info == "<=>")
        {
            left = eval(*T.children[0]);
            right = eval(*T.children[1]);
            
            if (left == right)
                return true;
            else
                return false;
        }
        return NULL;
    }
}

//Function used to call all other functions and assign the output of the expression
TPERslt TPE(string s)
{
    TPERslt result;
    
    tokRslt token = tokenize(s);
    
    parseRslt expression = parse(token.syms);
    
    //if it's a string of symbols
    if (token.success == true)
    {
        //if it's a proper boolean expression, evaluate it
        if (expression.success == true)
        {
            result.msg = "success";
            result.val = eval(expression.ast);
        }
        
        //if it's not, then there's a grammar error
        else if (expression.success == false)
        {
            result.msg = "grammar error";
        }
    }
    
    //if it's not a string of symbols, then there's a symbol error
    else if (token.success == false)
    {
        result.msg = "symbol error";
    }
    return result;
}

string TPEOut(string s)
{
    TPERslt output;
    
    output = TPE(s);
    
    //if TPE.msg is success
    if (output.msg == "success")
    {
        //output the result of the boolean expression
        if (output.val == true)
            return "true";
        else if (output.val == false)
            return "false";
        else
            return NULL;
    }
    
    //if TPE.msg is grammar error, display so
    else if (output.msg == "grammar error")
        return output.msg;
    
    //if TPE.msg is symbol error, display so
    else
        return output.msg;
}

int main()
{
    string user_input;
	
    cout << "Enter a boolean expression:\n";
    getline(cin, user_input);
    
    cout << TPEOut(user_input);
    
    return 0;
}