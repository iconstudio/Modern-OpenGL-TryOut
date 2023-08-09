module;
module Glib;
import <type_traits>;
import :VertexBuffer;

gl::VertexBuffer::VertexBuffer()
noexcept
	: myHandle(buffer::BufferType::Array)
{}

void
gl::VertexBuffer::Assign(std::initializer_list<std::int32_t> list, buffer::BufferUsage usage)
noexcept
{
	this->Assign(list.begin(), list.size(), usage);
}

void
gl::VertexBuffer::Assign(std::initializer_list<std::uint32_t> list, buffer::BufferUsage usage)
noexcept
{
	this->Assign(list.begin(), list.size(), usage);
}

void
gl::VertexBuffer::Assign(std::initializer_list<float> list, buffer::BufferUsage usage)
noexcept
{
	this->Assign(list.begin(), list.size(), usage);
}

void
gl::VertexBuffer::Assign(const void* const& data, const size_t& size, buffer::BufferUsage usage)
noexcept
{
	myHandle.SetData(data, size, usage);
}

void
gl::VertexBuffer::SetLayout(const BufferLayout& layout)
noexcept
{
	myHandle.SetLayout(layout);
}

void
gl::VertexBuffer::SetLayout(BufferLayout&& layout)
noexcept
{
	myHandle.SetLayout(std::move(layout));
}

void
gl::VertexBuffer::Bind()
const noexcept
{
	myHandle.Bind();
}

void
gl::VertexBuffer::Unbind()
const noexcept
{
	myHandle.Unbind();
}

void
gl::VertexBuffer::Use()
const noexcept
{
	myHandle.Use();
}

gl::buffer::BufferUsage
gl::VertexBuffer::GetUsage()
const noexcept
{
	return myHandle.GetUsage();
}

size_t
gl::VertexBuffer::GetSize()
const noexcept
{
	return myHandle.GetSize();

}

gl::BufferObject&
gl::VertexBuffer::GetHandle()
noexcept
{
	return myHandle;
}

const gl::BufferObject&
gl::VertexBuffer::GetHandle()
const noexcept
{
	return myHandle;
}