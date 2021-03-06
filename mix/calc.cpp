/**
 * пример реализации простого калькулятора командной строки
 *
 */
#include <iostream> // I/O
#include <string>   // строковые функции
#include <map>      // ассоциативные массивы "map"
#include <cctype>   // isalpha()  т.п.


using namespace std;

enum Token_value {
		NAME,	    NUMBER,      END,
		PLUS='+',   MINUS='-',   MUL='*',  DIV='/',
		PRINT=';',  ASSIGN='=',  LP='(',   RP=')'
};
map<string, double> table;
Token_value curr_tok = PRINT;
double number_value;
string string_value;
int no_of_errors = int();

int error(const string& s)
{
	cerr << "error: " << s << '\n';
	return ++no_of_errors;
}
double expr(bool);

Token_value get_token(void)
{
	char ch = 0;
	cin >> ch;

	switch(ch)
	{
		case 0:
			return curr_tok = END;
		case ';': case '*': case '/': case '+':	case '-': case '(':
		case ')': case '=':
			return curr_tok = Token_value(ch);
		case '0': case '1': case '2': case '3': case '4': case '5':
		case '6': case '7': case '8': case '9':	case '.':
			cin.putback(ch);
			cin >> number_value;
			return curr_tok = NUMBER;
		default:
			if(isalpha(ch))
			{
				cin.putback(ch);
				cin >> string_value;
				return curr_tok = NAME;
			}
			error("bad token");
			return curr_tok = PRINT;
	}
}

/* обработчик первичных выражений */
double prim(bool get)
{
	if(get) get_token();
	switch(curr_tok)
	{
		case NUMBER:
		{
			double v = number_value;
			get_token();
			return v;
		}
		case NAME:
		{
			double& v = table[string_value];
			if(get_token() == ASSIGN) v = expr(true);
			return v;
		}
		case MINUS:							// унарный минус
			return -prim(true);
		case LP:
		{
			double e = expr(true);
			if(curr_tok != RP) return error("')' expected");
			get_token();					// пропустить ')'
			return e;
		}
		default:
			return error("primary expected");
	}
}

/* обработка операторов умножения и деления */
double term(bool get)
{
	double left = prim(get);
	for(;;)
		switch(curr_tok)
		{
			case MUL:
				left *= prim(true);
				break;
			case DIV:
				if(double d = prim(true))	//защита от деления на 0
				{
					left /= d;
					break;
				}
				return error("divide by 0");
			default:
				return left;
	}
}

/* обработка сложения и вычитания */
double expr(bool get)
{
	double left = term(get);
	for(;;)
		switch(curr_tok)
		{
			case PLUS:
				left += term(true);
				break;
			case MINUS:
				left -= term(true);
				break;
			default:
				return left;
		}
}

int main()
{
	table["pi"]=3.141592653589793;
	table["e"]=2.718281828;

	while(cin){
		get_token();
		if(curr_tok == END) break;
		if(curr_tok == PRINT) continue;
		cout << expr(false) << '\n';
	}
	return no_of_errors;
}
