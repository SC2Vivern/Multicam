#pragma once
#include <vector>
namespace ArenaUI
{
	template<class T>
	class Stack
	{
	public:
		virtual ~Stack(){};

		static Stack<T>* GetInstance()
		{
			return &m_stack;
		};

		void Push(T in)
		{
			m_pQuery.push_back(in);
		};

		size_t Size()
		{
			return m_pQuery.size();
		};

		T GetAt(size_t index = 0)
		{
			if (m_pQuery.size() <= index)
			{
				throw(std::out_of_range("Index into stack is out of range"));
			}

			return m_pQuery[index];
		};

		void Clear()
		{
			return m_pQuery.clear();
		}

	private:
		Stack(){};

		static Stack<T> m_stack;
		std::vector<T> m_pQuery;
	};
} // namespace ArenaUI
