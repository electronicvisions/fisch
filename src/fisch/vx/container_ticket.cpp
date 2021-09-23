#include "fisch/vx/container_ticket.h"

#include "fisch/vx/container.h"
#include "fisch/vx/playback_program.h"
#include "fisch/vx/traits.h"
#include "hate/type_list.h"
#include "hxcomm/vx/utmessage.h"

#include <boost/iterator/iterator_facade.hpp>

namespace fisch::vx {

template <class ContainerT>
ContainerTicket<ContainerT>::ContainerTicket(
    size_t container_count, std::shared_ptr<detail::ContainerTicketStorage<ContainerT>> storage) :
    m_container_count(container_count), m_storage(std::move(storage))
{}

namespace detail {

/**
 * Random-access iterator for decoding a sequence of containers on the fly.
 * @tparam ContainerT Type of container
 * @tparam MessageIterator Random-access iterator for message sequence
 */
template <typename ContainerT, typename MessageIterator>
struct DecodeIterator
    : public boost::iterator_facade<
          DecodeIterator<ContainerT, MessageIterator>,
          ContainerT,
          boost::random_access_traversal_tag>
{
	DecodeIterator(MessageIterator const begin) : it(begin), config() {}

private:
	MessageIterator it;
	mutable ContainerT config;

	friend class boost::iterator_core_access;

	bool equal(DecodeIterator const& other) const
	{
		return it == other.it;
	}

	void decrement()
	{
		it -= ContainerT::decode_ut_message_count;
	}

	void increment()
	{
		it += ContainerT::decode_ut_message_count;
	}

	void advance(typename MessageIterator::difference_type const n)
	{
		it += n * ContainerT::decode_ut_message_count;
	}

	typename MessageIterator::difference_type distance_to(DecodeIterator const other) const
	{
		return std::distance(it, other.it) /
		       typename MessageIterator::difference_type(ContainerT::decode_ut_message_count);
	}

	ContainerT& dereference() const
	{
		config.decode({it, it + ContainerT::decode_ut_message_count});
		return config;
	}

	/**
	 * Ensure it is really a random access iterator to make use of efficient std::vector(begin, end)
	 * constructor.
	 */
	static_assert(std::is_base_of_v<
	              std::random_access_iterator_tag,
	              typename DecodeIterator::iterator_category>);
};

} // namespace detail

template <class ContainerT>
std::vector<ContainerT> ContainerTicket<ContainerT>::get() const
{
	if constexpr (IsReadable<ContainerT>::value) {
		if (!valid()) {
			throw std::runtime_error("Data not available.");
		}

		if (!m_storage) {
			throw std::runtime_error("Unexpected access to moved-from object.");
		}
		assert(m_storage->m_pbp);
		auto const& queue = std::get<detail::decode_message_types_index<ContainerT>>(
		    m_storage->m_pbp->m_receive_queue);

		auto const begin_it = queue.get_messages().cbegin() + m_storage->m_pos;
		typedef detail::DecodeIterator<ContainerT, std::remove_cv_t<decltype(begin_it)>> Iterator;
		return std::vector<ContainerT>(
		    Iterator(begin_it),
		    Iterator(begin_it + ContainerT::decode_ut_message_count * m_container_count));
	} else {
		throw std::logic_error("Container not readable.");
	}
}

template <class ContainerT>
bool ContainerTicket<ContainerT>::valid() const
{
	if constexpr (IsReadable<ContainerT>::value) {
		if (!m_storage) {
			throw std::runtime_error("Unexpected access to moved-from object.");
		}
		assert(m_storage->m_pbp);
		auto const& queue = std::get<detail::decode_message_types_index<ContainerT>>(
		    m_storage->m_pbp->m_receive_queue);
		return (
		    queue.size() >=
		    (m_storage->m_pos + (m_container_count * ContainerT::decode_ut_message_count)));
	} else {
		throw std::logic_error("Container not readable.");
	}
}

template <class ContainerT>
FPGATime ContainerTicket<ContainerT>::fpga_time() const
{
	if constexpr (IsReadable<ContainerT>::value) {
		if (!valid()) {
			throw std::runtime_error("Data not available.");
		}
		if (!m_storage) {
			throw std::runtime_error("Unexpected access to moved-from object.");
		}
		assert(m_storage->m_pbp);
		auto const& queue = std::get<detail::decode_message_types_index<ContainerT>>(
		    m_storage->m_pbp->m_receive_queue);
		return *(
		    queue.get_times().cbegin() + m_storage->m_pos +
		    m_container_count * ContainerT::decode_ut_message_count - 1);
	} else {
		throw std::logic_error("Container not readable.");
	}
}

// explicit instantiation
#define PLAYBACK_CONTAINER(Name, _Type) template class ContainerTicket<Name>;
#include "fisch/vx/container.def"

} // namespace fisch::vx
