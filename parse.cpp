/*
 * parse.cpp
 */

#include "parse.h"

// WRAPPER FOR PUSHBACK
namespace Parser {
bool pushed_back = false;
Token	pushed_token;

static Token GetNextToken(istream *in, int *line) {
	if( pushed_back ) {
		pushed_back = false;
		return pushed_token;
	}
	return getNextToken(in, line);
}

static void PushBackToken(Token& t) {
	if( pushed_back ) {
		abort();
	}
	pushed_back = true;
	pushed_token = t;
}

}

static int error_count = 0;

void
ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}

ParseTree *Prog(istream *in, int *line)
{
    /*
     * Prog Grammar Rule:
     * Prog := Slist
     */
    //Given so dont change it
	ParseTree *sl = Slist(in, line);

	if( sl == 0 )
		ParseError(*line, "No statements in program");

	if( error_count )
		return 0;

	return sl;
}

// Slist is a Statement followed by a Statement List
ParseTree *Slist(istream *in, int *line)
{
    /*
     * Slist Grammar Rule:
     * Slist := Stmt SC { Slist }
     */
    //Given so dont change it
	ParseTree *s = Stmt(in, line);

	if( s == 0 )
		return 0;

    //SC part - Semicolon
	if( Parser::GetNextToken(in, line) != SC ) {
		ParseError(*line, "Missing semicolon");
		return 0;
	}
    //Statement List
    return new StmtList(s, Slist(in,line));
}

ParseTree *Stmt(istream *in, int *line) {
    /*
     * Stmt Grammar Rule:
     * Stmt := IfStmt | PrintStmt | Expr
     */
    //Given so dont change it
	ParseTree *s;

	Token t = Parser::GetNextToken(in, line);
	switch( t.GetTokenType() ) {
	case IF:
		s = IfStmt(in, line);
		break;
	case PRINT:
		s = PrintStmt(in, line);
		break;
	case DONE:
		return 0;
	case ERR:
		ParseError(*line, "Invalid token");
		return 0;
	default:
		// put back the token and then see if it's an Expr
		Parser::PushBackToken(t);
		s = Expr(in, line);
		if( s == 0 ) {
			ParseError(*line, "Invalid statement");
			return 0;
		}
		break;
	}
	return s;
}

ParseTree *IfStmt(istream *in, int *line)
{
    /*
     * IfStmt Grammar Rule:
     * IfStmt := IF Expr THEN Stmt
     */
    //ParseTree for Expression
    ParseTree *ex = Expr(*in, *line);
    //Create a new token 't'
    Token t = Parser::GetNextToken(in, line);

    /*
     * We only need to check for a "THEN" statement
     * If theres a THEN, return it
     */
    if(t.GetTokenType() == THEN)
    {
        ParseTree *s = Stmt(*in, *line);
        return new IfStatement(t.GetLinenum(), ex, stmt);
    }
    return 0;
}

ParseTree *PrintStmt(istream *in, int *line)
{
    /*
     * PrintStmt Grammar Rule:
     * PrintStmt := PRINT Expr
     */
    //Create new ParseTree for *PrintStmt
    ParseTree *PrtStmt = Expr(*in, *line);

    //Given by Professor, dont change this line below
	return new PrintStatement(l, ex);
}

ParseTree *Expr(istream *in, int *line)
{
    /*
     * Expr Grammar Rule:
     * Expr := LogicExpr { ASSIGN LogicExpr }
     */
    /*
     * (Maybe? For now) This Parts Done Already, no need to change
     */
	ParseTree *t1 = LogicExpr(in, line);
	if( t1 == 0 ) {
        ParseError(*line, "Error at Expression");
		return 0;
	}
	Token t = Parser::GetNextToken(in, line);

	if( t != ASSIGN ) {
		Parser::PushBackToken(t);
		return t1;
	}

	ParseTree *t2 = Expr(in, line); // right assoc
	if( t2 == 0 ) {
		ParseError(*line, "Missing expression after operator");
		return 0;
	}

	return new Assignment(t.GetLinenum(), t1, t2);
}

ParseTree *LogicExpr(istream *in, int *line)
{
    /*
     * LogicExpr Grammar Rule:
     * LogicExpr := CompareExpr { (LOGICAND | LOGICOR) CompareExpr }
     */
	ParseTree *t1 = CompareExpr(in, line);
	if( t1 == 0 ) {
        ParseError(*line, "Error at Logic Expression - t1");
		return 0;
	}

    Token t = Parser::GetNextToken(in, line);
	if(t.GetTokenType() != LOGICAND && t.GetTokenType() != LOGICOR)
    {
        Parser::PushBackToken(t);
        return t1;
    }

    ParseTree *t2 = CompareExpr(in, line);
    if(t2 == 0)
    {
        ParseError(*line, "Error at Logic Expression - t2");
        return 0;
    }

    //LogicAndExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}

    if( t == LOGICAND )
        t1 = new LogicAndExpr(t.GetLinenum(), t1, t2);
    else if (t == LOGICOR)
        t1 = new LogicOrExpr(t.GetLinenum(), t1, t2);

    return t1;
}

ParseTree *CompareExpr(istream *in, int *line) {
	ParseTree *t1 = AddExpr(in, line);
	if( t1 == 0 )
	{
        ParseError(*line, "Error at Compare Expression - 1");
		return 0;
	}

    Token t = Parser::GetNextToken(in, line);
    if(t.GetTokenType() != EQ && t.GetTokenType() != NEQ && t.GetTokenType() != GT && t.GetTokenType() != LT && t.GetTokenType() != LEQ)
    {
        Parser::PushBackToken(t);
        return t1;
    }


    //Do Checks
    if( t == EQ )
    {
        //EqExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
        t1 = new EqExpr(t.GetLinenum(), t1, t2);
    }
    else if (t == NEQ)
    {
        //	NEqExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
        t1 = new NEqExpr(t.GetLinenum(), t1, t2);
    }
    else if (t == GT)
    {
        //GtExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
        t1 = new GtExpr(t.GetLinenum(), t1, t2);
    }
    else if (t == LT)
    {
        //LtExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
        t1 = new LtExpr(t.GetLinenum(), t1, t2);
    }
    else if (t == LEQ)
    {
        //LEqExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
        t1 == new LEqExpr(t.GetLinenum(), t1, t2);
    }

    return 0;
}

ParseTree *AddExpr(istream *in, int *line) {
    //Part Done Already - DO NOT TOUCH
	ParseTree *t1 = MulExpr(in, line);
	if( t1 == 0 ) {
		return 0;
	}

	while ( true ) {
		Token t = Parser::GetNextToken(in, line);

		if( t != PLUS && t != MINUS ) {
			Parser::PushBackToken(t);
			return t1;
		}

		ParseTree *t2 = MulExpr(in, line);
		if( t2 == 0 ) {
			ParseError(*line, "Missing expression after operator");
			return 0;
		}

		if( t == PLUS )
			t1 = new PlusExpr(t.GetLinenum(), t1, t2);
		else
			t1 = new MinusExpr(t.GetLinenum(), t1, t2);
	}
}

ParseTree *MulExpr(istream *in, int *line) {
	ParseTree *t1 = Factor(in, line);
	if( t1 == 0 ) {
		return 0;
	}

    // HANDLE OP
    return 0;
}

ParseTree *Factor(istream *in, int *line) {
	bool neg = false;
	Token t = Parser::GetNextToken(in, line);

	if( t == MINUS ) {
		neg = true;
	}
	else {
		Parser::PushBackToken(t);
	}

	ParseTree *p1 = Primary(in, line);
	if( p1 == 0 ) {
		ParseError(*line, "Missing primary");
		return 0;
	}

	if( neg ) {
        // handle as -1 * Primary
		return new TimesExpr(t.GetLinenum(), new IConst(t.GetLinenum(), -1), p1);
	}
	else
		return p1;
}

ParseTree *Primary(istream *in, int *line) {
	Token t = Parser::GetNextToken(in, line);

    // PROCESS TOKEN, IDENTIFY PRIMARY, RETURN SOMETHING
	return 0;
}

