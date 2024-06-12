#pragma once

#include <string>
#include <map>

//---------------------------------------------------------------------------
class eGlDrawContext
{
public:
	eGlDrawContext() {}
	eGlDrawContext(const eGlDrawContext&) = delete;
	eGlDrawContext& operator=(eGlDrawContext&) = delete;

	uint32_t GetDrawCallsCount() { return m_total_draw_calls; }
	uint32_t GetDrawTrianglesCount() { return m_total_triangles; }

	void Flush() { m_tag_calls.clear();  m_total_draw_calls = 0; m_total_triangles = 0; }

	void DrawElements(GLenum _mode, GLsizei _count, GLenum _type, const void* _indices, const std::string& _tag);
	void DrawElementsInstanced(GLenum _mode, GLsizei _count, GLenum _type, const void* _indices, GLsizei _instances, const std::string& _tag);
	void DrawArrays(GLenum _mode, GLint _first, GLsizei _count, const std::string& _tag);

	static eGlDrawContext& GetInstance()
	{
		static eGlDrawContext  instance;
		return instance;
	}

protected:
	void _Debug(const std::string& _tag, GLsizei _triangles)
	{
		if (auto it = m_tag_calls.find(_tag); it != m_tag_calls.end())
			m_tag_calls.insert_or_assign(_tag, ++(it->second));
		else
			m_tag_calls.insert({ _tag, 1 });

		++m_total_draw_calls;
		m_total_triangles += _triangles;
	}

	bool															m_debuging = true;
	std::map<std::string, uint32_t>		m_tag_calls;
	uint32_t													m_total_draw_calls = 0;
	GLsizei														m_total_triangles = 0;
};