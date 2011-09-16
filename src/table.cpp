#include "table.h"

#include "string_utils.h"
#include "table_entry.h"
#include "wireformat.h"

#include <sstream>
#include <stdexcept>

namespace amqpp
{

void table::insert(const table_entry& data)
{
  m_table.insert(std::make_pair(data.get_key(), data));
}

std::string table::to_string() const
{
  std::ostringstream os;
  os << "{ len: " << m_table.size();
  for (table_impl_t::const_iterator it = m_table.begin();
        it != m_table.end(); ++it)
  {
    os << ", " << it->second.to_string();
  }
  os << "}";
  return os.str();
}

uint32_t table::wireformat_size() const
{
  uint32_t size = sizeof(uint32_t); // header
  for (table_impl_t::const_iterator it = m_table.begin();
       it != m_table.end(); ++it)
  {
    size += it->second.wireformat_size(); // Table entry value
  }
  return size;
}


} // namespace amqpp
