## 背景

客户端a把码流往客户端b发。

码流都是封装到自己定义的协议里面。

如果是自定义协议的情况

|头部|数据区域|，对于数据区域怎么封装packet进行解码。比如h264  自己去判断00 00 00 01或者00 00 01,

自己根据startcode去进行分帧。NALU  NALU  

自定义io  avio_alloc_context

## 内存IO模式：avio_alloc_context()

```
/**
 * Allocate and initialize an AVIOContext for buffered I/O. It must be later
 * freed with avio_context_free().
 *
 * @param buffer Memory block for input/output operations via AVIOContext.
 *        The buffer must be allocated with av_malloc() and friends.
 *        It may be freed and replaced with a new buffer by libavformat.
 *        AVIOContext.buffer holds the buffer currently in use,
 *        which must be later freed with av_free().
 * @param buffer_size The buffer size is very important for performance.
 *        For protocols with fixed blocksize it should be set to this blocksize.
 *        For others a typical size is a cache page, e.g. 4kb.
 * @param write_flag Set to 1 if the buffer should be writable, 0 otherwise.
 * @param opaque An opaque pointer to user-specific data.
 * @param read_packet  A function for refilling the buffer, may be NULL.
 *                     For stream protocols, must never return 0 but rather
 *                     a proper AVERROR code.
 * @param write_packet A function for writing the buffer contents, may be NULL.
 *        The function may not change the input buffers content.
 * @param seek A function for seeking to specified byte position, may be NULL.
 *
 * @return Allocated AVIOContext or NULL on failure.
 */
AVIOContext *avio_alloc_context(
                  unsigned char *buffer,
                  int buffer_size,
                  int write_flag,
                  void *opaque,
                  int (*read_packet)(void *opaque, uint8_t *buf, int buf_size),
                  int (*write_packet)(void *opaque, uint8_t *buf, int buf_size),
                  int64_t (*seek)(void *opaque, int64_t offset, int whence));
```

buffer，对于自定义io, 是自己分配的内存

buffer_size，分配的内存的大小

write_flag，缓冲区读写标志，读写是对于ffmpeg而言

​		write_flag为1时，buffer用于写，即作为ffmpeg输出

​		write_flag为0时，buffer用于读，即作为ffmpeg输入

opaque，是read_packet/write_packet的第一个参数，指向用户数据

read_packet和write_packet是函数指针，指向⽤户编写的回调函数

seek也是函数指针，需要⽀持seek时使⽤。 可以类⽐fseek的机制

## 测试演示

1.传入参数  包括程序本身是3个参数

​	输入文件（aac/mp3）, 输出文件（pcm）

2.自定义io

​	avio_alloc_context

3.avformat_open_input 打开文件，把自定义io做绑定。avformat_open_input读取数据的时候

​	不会把数据丢掉。av_read_frame真正把数据读走。

4.查找指定编码器

5.循环读取 packet进行解码，然后存储到输出文件