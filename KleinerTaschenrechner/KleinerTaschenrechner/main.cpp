#include <string>
#include <vector>



struct Token {
	enum class Type {
		number,		// any size number
		add,		// +
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
	while (currentIndex < formel.size()) {
		char c = formel[currentIndex];
		currentIndex++;

		//space is not relevant
		if (c == ' ')
			continue;

		//pares all non number tokens
		if (c == '+') {
			toRet.tokens.push_back(new Token(Token::Type::add, currentPrio + 0));
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

	return toRet;
}

float calculate(Compilat compilat) {
	std::vector<Token*> tokenCopy;
	tokenCopy.reserve(compilat.tokens.size());
	for (Token* token : compilat.tokens) tokenCopy.push_back(token);

	for (uint16_t currentPrio = compilat.highestPrio; currentPrio < 0xFFFF; currentPrio--) {	//0xFFFF größte zahil in uint16_t
		for (uint16_t i = 0; i < tokenCopy.size(); i++) {
			Token* token = tokenCopy[i];
			if (token->priority != currentPrio)
				continue;

			uint16_t offset = 0;
			uint16_t used = 0;
			if (token->type == Token::Type::add) {
				Number* left = (Number*)tokenCopy[i - 1];
				Number* right = (Number*)tokenCopy[i + 1];
				left->value = left->value + right->value;
				offset = 0;
				used = 2;
			}

			for (uint16_t j = i + offset; j < tokenCopy.size() - used; j++)
				tokenCopy[j] = tokenCopy[j + used];
			tokenCopy.resize(tokenCopy.size() - used);
		}
	}


	return ((Number*)tokenCopy[0])->value;
}

int main() {
	std::string formel = "55555 + 10";
	Compilat compilat = compile(formel);
	float result = calculate(compilat);
	return 0;
}
