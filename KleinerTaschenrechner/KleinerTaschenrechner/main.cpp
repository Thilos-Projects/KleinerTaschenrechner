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
	};
	Type type;
	uint16_t priority;
	Token(Type type, uint16_t priority) : type(type), priority(priority) {}
};

struct Number : public Token {
	float value;
	Number(float value) : Token(Token::Type::number, 0), value(value) {}
};

struct Compilat {
	uint16_t highestPrio;
	std::vector<Token*> tokens;
	Compilat() : highestPrio(0) {}
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

		if (c == '(') {
			currentPrio+= maxPrio;
			
			continue;
		}

		if (c == ')') {
			currentPrio -= maxPrio;
			
			continue;
		}


		if (formel.size() - currentIndex > 1) {
			if (c == 's' && formel[currentIndex] == 'i' && formel[currentIndex+1] == 'n') {
				toRet.tokens.push_back(new Token(Token::Type::sin, currentPrio + 2));
				if (highestPrio < currentPrio + 2)
					highestPrio = currentPrio + 2;
				continue;
			}
		}

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

		toRet.tokens.push_back(new Number(number));
		continue;
	}
	toRet.highestPrio = highestPrio;
	return toRet;
}

float calculate(Compilat compilat) {
	std::vector<Token*> tokenCopy;
	std::vector<Token*> toDel;
	tokenCopy.reserve(compilat.tokens.size());
	for (Token* token : compilat.tokens) tokenCopy.push_back(token);

	for (uint16_t currentPrio = compilat.highestPrio; currentPrio < 0xFFFF; currentPrio--) {	//0xFFFF größte zahil in uint16_t
		for (uint16_t i = 0; i < tokenCopy.size(); i++) {
			Token* token = tokenCopy[i];
			if (token->priority != currentPrio)
				continue;

			if (token->type == Token::Type::number)
				continue;

			uint16_t offset = 0;
			uint16_t used = 0;

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

			for (uint16_t j = i + offset; j < tokenCopy.size() - used; j++)
				tokenCopy[j] = tokenCopy[j + used];
			tokenCopy.resize(tokenCopy.size() - used);
			i--;
		}
	}

	float toRet = ((Number*)tokenCopy[0])->value;
	for (uint16_t i = 0; i < toDel.size(); i++){
		free(toDel[i]);
	}
	return toRet;
}

int main() {
	std::string formel = "2+sin10";
	Compilat compilat = compile(formel);
	float result = calculate(compilat);
	return 0;
}
