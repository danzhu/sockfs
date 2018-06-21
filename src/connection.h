#ifndef SOCK_CONNECTION_H
#define SOCK_CONNECTION_H

class Fd;

class Connection
{
public:
    virtual ~Connection();
    virtual void on_read() = 0;
    virtual void on_error();

    virtual const Fd &fd() const = 0;
};

#endif
