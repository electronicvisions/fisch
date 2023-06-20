#pragma once
#include <utility>

namespace fisch::vx {

template <typename Container>
struct BackEmplacer
{
	BackEmplacer(Container& container) : m_container(container) {}

	template <typename... Args>
	void operator()(Args&&... args)
	{
		m_container.emplace_back(std::forward<Args>(args)...);
	}

private:
	Container& m_container;
};

} // namespace fisch::vx
