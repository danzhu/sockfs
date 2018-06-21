#include "connection.h"

#include <stdexcept>

Connection::~Connection() {}

void Connection::on_error() { throw std::runtime_error{"connection error"}; }
