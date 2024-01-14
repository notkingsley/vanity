//
// Created by kingsli on 9/20/23.
//

#ifndef VANITY_ARGUMENTS_H
#define VANITY_ARGUMENTS_H

#include <map>
#include <set>
#include <string>
#include <vector>

#include "exceptions.h"

namespace vanity{

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

	// multiple keyword arguments
	std::map<std::string, std::vector<std::string>> m_multi_kwargs;

	// add an argument
	void add_arg(std::string arg) {
		m_args.emplace(std::move(arg));
	}

	// add a keyword argument
	void add_kwarg(std::string key, std::string value) {
		if (m_multi_kwargs.contains(key)) {
			m_multi_kwargs.at(key).emplace_back(std::move(value));
			return;
		}

		if (not m_kwargs.contains(key)) {
			m_kwargs.emplace(std::move(key), std::move(value));
			return;
		}

		m_multi_kwargs.emplace(key, std::vector<std::string>{m_kwargs.at(key), std::move(value)});
		m_kwargs.erase(key);
	}

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

	// extract a key=value argument or key argument
	void extract_argument(const std::string& arg, size_t& pos){
		auto key = extract_word(arg, pos);
		if (arg[pos] == '\0')
			return add_arg(std::move(key));

		++pos;
		if (arg[pos] == '\0')
			throw MalformedArgument("expected value after = at " + key);

		auto value = extract_word(arg, pos);
		if (arg[pos] != '\0')
			throw MalformedArgument("unexpected = after -" + value);

		add_kwarg(std::move(key), std::move(value));
	}

	// extract the argument
	void parse_arg(const std::string& arg) {
		if (arg.compare(0, 2, "--") != 0)
			throw MalformedArgument("expected -- at start of argument");

		size_t pos = 2;
		if (arg[pos] == '\0')
			throw MalformedArgument("expected argument after --");

		extract_argument(arg, pos);
	}

public:
	Arguments() = default;
	~Arguments() = default;

	Arguments(int argc, char **argv) {
		parse(argc, argv);
	}

	// parse arguments
	void parse(int argc, char **argv) {
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

	// check if a multi keyword argument is present
	bool has_multi_kwarg(const std::string& key) const {
		return m_multi_kwargs.contains(key);
	}

	// get a keyword argument
	const std::string& get_kwarg(const std::string& key) const {
		return m_kwargs.at(key);
	}

	// get a multi keyword argument
	const std::vector<std::string>& get_multi_kwarg(const std::string& key) const {
		return m_multi_kwargs.at(key);
	}
};

} // namespace vanity

#endif //VANITY_ARGUMENTS_H
