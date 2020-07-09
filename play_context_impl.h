#ifndef __DEF_PLAY_CONTEXT_IMPL_H__
#define __DEF_PLAY_CONTEXT_IMPL_H__


class play_context_impl
{
public:
    explicit play_context_impl();
    virtual ~play_context_impl();

public:
    virtual bool create(void* view);
    virtual void destroy();
    virtual void play(const char* url);

private:

};

#endif // __DEF_PLAY_CONTEXT_IMPL_H__
