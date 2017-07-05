#pragma once

struct noncopyable 
{
protected:
    noncopyable() {}
    virtual ~noncopyable() {}

private:
    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(const noncopyable&) = delete;
    noncopyable(noncopyable&&) = delete;
    noncopyable& operator=(noncopyable&&) = delete;
};