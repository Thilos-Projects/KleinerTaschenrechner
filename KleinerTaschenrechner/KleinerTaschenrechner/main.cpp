#include <string>
#include <vector>

struct Token {
	enum class Type {
	};
	Type type;
	uint16_t priority;
};

struct Compilat {
	uint16_t highestPrio;
	std::vector<Token> tokens;
};

Compilat compile(std::string formel) {

}

float calculate(Compilat compilat) {
	return 0.0f;
}

int main() {
	std::string formel = "";
	Compilat compilat = compile(formel);
	float result = calculate(compilat);
	return 0;
}