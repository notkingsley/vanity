//
// Created by kingsli on 9/20/23.
//

#include <map>
#include <set>
#include <string>

#ifndef VANITY_ARGUMENTS_H
#define VANITY_ARGUMENTS_H

namespace vanity{

/*
 * Thrown for malformed arguments
 */
class MalformedArgument : public std::exception
{
private:
	std::string m_msg;

public:
	explicit MalformedArgument(std::string msg) : m_msg{std::move(msg)} {}
	const char* what() const noexcept override { return m_msg.c_str(); }
};

/*
 * A simple class to parse command line arguments
 */
class Arguments
{
private:
	// the arguments
	std::set<std::string> m_args;

	// keyword arguments
	std::map<std::string, std::string> m_kwargs;

	// extract till a '=' is found
	static std::string extract_word(const std::string& arg, size_t& pos)
	{
		std::string word;
		for (; arg[pos]; ++pos) {
			if (arg[pos] == '=')
				break;

			if (arg[pos] == '-')
				word += '_';
			else
				word += arg[pos];
		}
		return word;
	}

	// extract a -word argument
	void extract_short_argument(const std::string& arg, size_t& pos){
		auto word = extract_word(arg, pos);
		if (arg[pos] != '\0') {
			throw MalformedArgument("unexpected = after -" + word);
		}
		m_args.emplace(std::move(word));
	}

	// extract a --key=value argument or --key argument
	void extract_long_argument(const std::string& arg, size_t& pos){
		auto key = extract_word(arg, pos);
		if (arg[pos] == '\0') {
			m_args.emplace(std::move(key));
			return;
		}

		++pos;
		if (arg[pos] == '\0') {
			throw MalformedArgument("expected value after = at " + key);
		}

		auto value = extract_word(arg, pos);
		if (arg[pos] != '\0') {
			throw MalformedArgument("unexpected = after -" + value);
		}
		m_kwargs.emplace(std::move(key), std::move(value));
	}

	// extract the argument
	void parse_arg(const std::string& arg) {
		size_t pos = 0;
		if (arg[pos] == '-'){
			++pos;
			if (arg[pos] == '\0') {
				throw MalformedArgument("expected argument after -");
			}
			if (arg[pos] == '-'){
				++pos;
				if (arg[pos] == '\0') {
					throw MalformedArgument("expected argument after --");
				}
				return extract_long_argument(arg, pos);
			}
			return extract_short_argument(arg, pos);
		}
		throw MalformedArgument("expected - or --");
	}

public:
	Arguments() = default;
	~Arguments() = default;

	Arguments(int argc, char **argv){
		parse(argc, argv);
	}

	// parse arguments
	void parse(int argc, char **argv){
		for (int i = 1; i < argc; ++i)
			parse_arg(argv[i]);
	}

	// check if an argument is present
	bool has(const std::string& key) const {
		return m_args.contains(key);
	}

	// check if a keyword argument is present
	bool has_kwarg(const std::string& key) const {
		return m_kwargs.contains(key);
	}

	// get a keyword argument
	const std::string& get_kwarg(const std::string& key) const {
		return m_kwargs.at(key);
	}
};

} // namespace vanity

#endif //VANITY_ARGUMENTS_H
