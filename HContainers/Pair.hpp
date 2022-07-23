#pragma once


template<typename T1, typename T2>
struct Pair
{
	T1 first{};
	T2 second{};

	Pair() {}
	Pair(T1 f, T2 s) : first(f), second(s) {}
	
	bool operator == (const Pair& other) const{ return first == other.first && second == other.second; }
	bool operator != (const Pair& other) const{ return first != other.first || second != other.second; }
};

template<typename Key, typename Value>
struct KeyValuePair
{
	Key key{};
	Value value{};

	KeyValuePair() {}
	KeyValuePair(Key k, Value v) : key(k), value(v) {}

	bool operator == (const KeyValuePair& other) const { return key == other.key && value == other.value; }
	bool operator != (const KeyValuePair& other) const { return key != other.key || value != other.value; }
};

