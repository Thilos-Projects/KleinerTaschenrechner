#include <string>
#include <vector>


#define maxPrio 3
struct Token {
	enum class Type {
		number,		// any size number
		add,		// +
		subtract,   // -
		mult,		// *
		div,		// /
		sin,		// sin
		fac,		// !
	};
	Type type;
	uint16_t priority;
	Token(Type type, uint16_t priority) : type(type), priority(priority) {}
};

struct Number : public Token {
	float value;
	Number(float value) : Token(Token::Type::number, 0), value(value) {}
};

struct TokenTyp;

TokenTyp* numberToken;
std::vector<TokenTyp*> alleToken;

struct Compilat {
	struct InternToken {
		TokenTyp* tokenTyp;
		uint32_t priority;
		float number;
		InternToken() :tokenTyp(nullptr), priority(0), number(0) {}
		InternToken(TokenTyp* typ, uint32_t priority, float number) : tokenTyp(typ), priority(priority), number(number) {}
	};
	uint16_t highestPrio;
	std::vector<InternToken> tokens;
	Compilat() : highestPrio(0) {}
};

struct TokenTyp {
	uint8_t prio;
	std::string trennzeichen;
	TokenTyp(std::string temp, uint8_t prio) : trennzeichen(temp), prio(prio) {
		alleToken.push_back(this);
	}
	virtual bool calculate(std::vector<Compilat::InternToken>* tokens, uint16_t currentIndex, uint16_t* offset, uint16_t* used) = 0;
};

float fac(float input) {
	if (input <= 1) {
		return input;
	}
	else
	{
		return input * fac(input-1);
	}
}

struct NumberToken :public TokenTyp {
	NumberToken() :TokenTyp("", 0) {
		numberToken = this;
	}
	bool calculate(std::vector<Compilat::InternToken>* tokens, uint16_t currentIndex, uint16_t* offset, uint16_t* used) {
		return false;
	}
};
struct AddToken : public TokenTyp {
	AddToken() :TokenTyp("+", 0) {}
	bool calculate(std::vector<Compilat::InternToken>* tokens, uint16_t currentIndex, uint16_t* offset, uint16_t* used) {
		tokens->at(currentIndex - 1).number = tokens->at(currentIndex - 1).number + tokens->at(currentIndex + 1).number;
		*offset = 0;
		*used = 2;
		return true;
	}
};
struct SubToken :public TokenTyp {
	SubToken() :TokenTyp("-", 0) {}
	bool calculate(std::vector<Compilat::InternToken>* tokens, uint16_t currentIndex, uint16_t* offset, uint16_t* used) {
		tokens->at(currentIndex - 1).number = tokens->at(currentIndex - 1).number - tokens->at(currentIndex + 1).number;
		*offset = 0;
		*used = 2;
		return true;
	}
};
struct MulToken :public TokenTyp {
	MulToken() :TokenTyp("*", 0) {}
	bool calculate(std::vector<Compilat::InternToken>* tokens, uint16_t currentIndex, uint16_t* offset, uint16_t* used) {
		tokens->at(currentIndex - 1).number = tokens->at(currentIndex - 1).number * tokens->at(currentIndex + 1).number;
		*offset = 0;
		*used = 2;
		return true;
	}
};
struct SinToken :public TokenTyp {
	SinToken() :TokenTyp("sin", 2) {}
	bool calculate(std::vector<Compilat::InternToken>* tokens, uint16_t currentIndex, uint16_t* offset, uint16_t* used) {
		tokens->at(currentIndex).number = sin(tokens->at(currentIndex + 1).number);
		tokens->at(currentIndex).tokenTyp = numberToken;
		*offset = 1;
		*used = 1;
		return true;
	}
};

Compilat compile(std::string formel) {
	Compilat toRet;
	uint16_t currentIndex = 0;
	uint16_t currentPrio = 0;
	uint16_t highestPrio = 0;
	while (currentIndex < formel.size()) {
		char c = formel[currentIndex];
		currentIndex++;

		//space is not relevant
		if (c == ' ')
			continue;

		//brackets

		if (c == '(') {
			currentPrio+= maxPrio;
			
			continue;
		}

		if (c == ')') {
			currentPrio -= maxPrio;
			
			continue;
		}

		bool found0 = false;
		for (uint8_t i = 0; i < alleToken.size(); i++) {
			if (formel.size() - currentIndex > alleToken[i]->trennzeichen.size() - 1) {
				bool found1 = alleToken[i]->trennzeichen.size() != 0;
				for (uint8_t j = 0; j < alleToken[i]->trennzeichen.size(); j++) {
					if (formel[currentIndex - 1 + j] != alleToken[i]->trennzeichen[j]) {
						found1 = false;
						break;
					}
				}
				if (!found1)
					continue;

				toRet.tokens.push_back(Compilat::InternToken(alleToken[i], currentPrio + alleToken[i]->prio, 0));
				if (highestPrio < currentPrio + alleToken[i]->prio)
					highestPrio = currentPrio + alleToken[i]->prio;
				found0 = true;
				currentIndex += alleToken[i]->trennzeichen.size() - 1;
				break;
			}
		}
		if (found0)
			continue;

		/*
		// sinus
		if (formel.size() - currentIndex > 1) {
			if (c == 's' && formel[currentIndex] == 'i' && formel[currentIndex+1] == 'n') {
				toRet.tokens.push_back(new Token(Token::Type::sin, currentPrio + 2));
				if (highestPrio < currentPrio + 2)
					highestPrio = currentPrio + 2;
				currentIndex += 2;
				continue;
			}
		}
		*/
		 /*
		//pares all non number tokens
		if (c == '+') {
			toRet.tokens.push_back(new Token(Token::Type::add, currentPrio + 0));
			if (highestPrio < currentPrio + 0)
				highestPrio = currentPrio + 0;
			continue;
		}

		if (c == '-') {
			toRet.tokens.push_back(new Token(Token::Type::subtract, currentPrio + 0));
			if (highestPrio < currentPrio + 0)
				highestPrio = currentPrio + 0;
			continue;
		}

		if (c == '*') {
			toRet.tokens.push_back(new Token(Token::Type::mult, currentPrio + 1));
			if (highestPrio < currentPrio + 1)
				highestPrio = currentPrio + 1;
			continue;
		}

		if (c == '/') {
			toRet.tokens.push_back(new Token(Token::Type::div, currentPrio + 1));
			if (highestPrio < currentPrio + 1)
				highestPrio = currentPrio + 1;
			continue;
		}

		if (c == '!') {
			toRet.tokens.push_back(new Token(Token::Type::fac, currentPrio + 1));
			if (highestPrio < currentPrio + 1)
				highestPrio = currentPrio + 1;
			continue;
		}
		*/

		//parse numbers
		float number = 0;
		number += c - '0';		// achtung dürfen nur ziffern sein
		while(currentIndex < formel.size()) {
			c = formel[currentIndex];

			if (c < '0' || c > '9')
				break;

			number *= 10;
			number += c - '0';
			currentIndex++;
		}

		toRet.tokens.push_back(Compilat::InternToken(numberToken, 0, number));
		continue;
	}
	toRet.highestPrio = highestPrio;
	return toRet;
}

float calculate(Compilat compilat) {
	std::vector<Compilat::InternToken> tokenCopy;
	std::vector<Compilat::InternToken> toDel;
	tokenCopy.reserve(compilat.tokens.size());
	for (Compilat::InternToken token : compilat.tokens) tokenCopy.push_back(token);

	for (uint16_t currentPrio = compilat.highestPrio; currentPrio < 0xFFFF; currentPrio--) {	//0xFFFF größte zahil in uint16_t
		for (uint16_t i = 0; i < tokenCopy.size(); i++) {
			Compilat::InternToken token = tokenCopy[i];
			if (token.priority != currentPrio)
				continue;

			uint16_t offset = 0;
			uint16_t used = 0;
			if(!token.tokenTyp->calculate(&tokenCopy, i, &offset, &used))
				continue;

			for (uint16_t j = i + offset; j < tokenCopy.size() - used; j++)
				tokenCopy[j] = tokenCopy[j + used];
			tokenCopy.resize(tokenCopy.size() - used);
			i--;

			//if (token.tokenTyp == numberToken)
			//	continue;

			//uint16_t offset = 0;
			//uint16_t used = 0;

			/*
			if (token->type == Token::Type::sin) {
				Number* me = new Number(0);
				Number* right = (Number*)tokenCopy[i + 1];
				tokenCopy[i] = me;
				me->value = sin(right->value);
				offset = 1;
				used = 1;
				toDel.push_back(me);
			}

			if (token->type == Token::Type::add) {
				Number* left = (Number*)tokenCopy[i - 1];
				Number* right = (Number*)tokenCopy[i + 1];
				left->value = left->value + right->value;
				offset = 0;
				used = 2;
			}

			if (token->type == Token::Type::subtract) {
				Number* left = (Number*)tokenCopy[i - 1];
				Number* right = (Number*)tokenCopy[i + 1];
				left->value = left->value - right->value;
				offset = 0;
				used = 2;
			}

			if (token->type == Token::Type::mult) {
				Number* left = (Number*)tokenCopy[i - 1];
				Number* right = (Number*)tokenCopy[i + 1];
				left->value = left->value * right->value;
				offset = 0;
				used = 2;
			}

			if (token->type == Token::Type::div) {
				Number* left = (Number*)tokenCopy[i - 1];
				Number* right = (Number*)tokenCopy[i + 1];
				left->value = left->value / right->value;
				offset = 0;
				used = 2;
			}

			if (token->type == Token::Type::fac) {
				Number* left = (Number*)tokenCopy[i - 1];
				Number* me = new Number(0);
				tokenCopy[i] = me;
				left->value = fac(left->value);
				offset = 0;
				used = 1;
				toDel.push_back(me);
			}*/

			//for (uint16_t j = i + offset; j < tokenCopy.size() - used; j++)
			//	tokenCopy[j] = tokenCopy[j + used];
			//tokenCopy.resize(tokenCopy.size() - used);
			//i--;
		}
	}

	return tokenCopy[0].number;
}

int main() {
	NumberToken nT = NumberToken();
	AddToken aT = AddToken();
	SubToken sT = SubToken();
	MulToken mT = MulToken();
	SinToken sinT = SinToken();

	std::string formel = "sin3";
	Compilat compilat = compile(formel);
	float result = calculate(compilat);
	return 0;
}
