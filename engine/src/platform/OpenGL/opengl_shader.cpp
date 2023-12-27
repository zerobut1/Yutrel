// #include "yutrel_pch.hpp"

// #include "opengl_shader.hpp"

// #include <glad/glad.h>
// #include <glm/gtc/type_ptr.hpp>
// #include <stdint.h>
// #include <string>

// namespace Yutrel
// {

//     // 当前绑定的shader
//     static GLuint cur_bind_shader = 0;

//     //////////////////////////////////////////////
//     // Shader
//     //////////////////////////////////////////////

//     OpenGLShader::OpenGLShader(const std::string& vertex_path, const std::string& fragment_path)
//     {
//         std::string vertex_src   = ReadFile(vertex_path);
//         std::string fragment_src = ReadFile(fragment_path);

//         std::unordered_map<GLenum, std::string> sources;
//         sources[GL_VERTEX_SHADER]   = vertex_src;
//         sources[GL_FRAGMENT_SHADER] = fragment_src;
//         Compile(sources);

//         // 从path中取得文件名，作为shader的名字
//         auto lastSlash = fragment_path.find_last_of("/\\");
//         lastSlash      = lastSlash == std::string::npos ? 0 : lastSlash + 1;
//         auto lastDot   = fragment_path.rfind('.');
//         auto count     = lastDot == std::string::npos ? fragment_path.size() - lastSlash : lastDot - lastSlash;
//         m_name         = fragment_path.substr(lastSlash, count);

//         LOG_INFO("Compile Shader: {0}", m_name);
//     }

//     OpenGLShader::OpenGLShader(const std::string& vertex_path, const std::string& fragment_path, const std::string& geometry_path)
//     {
//         std::string vertex_src   = ReadFile(vertex_path);
//         std::string fragment_src = ReadFile(fragment_path);
//         std::string geometry_src = ReadFile(geometry_path);

//         std::unordered_map<GLenum, std::string> sources;
//         sources[GL_VERTEX_SHADER]   = vertex_src;
//         sources[GL_FRAGMENT_SHADER] = fragment_src;
//         sources[GL_GEOMETRY_SHADER] = geometry_src;

//         Compile(sources);

//         // 从path中取得文件名，作为shader的名字
//         auto lastSlash = fragment_path.find_last_of("/\\");
//         lastSlash      = lastSlash == std::string::npos ? 0 : lastSlash + 1;
//         auto lastDot   = fragment_path.rfind('.');
//         auto count     = lastDot == std::string::npos ? fragment_path.size() - lastSlash : lastDot - lastSlash;
//         m_name         = fragment_path.substr(lastSlash, count);

//         LOG_INFO("Compile Shader: {0}", m_name);
//     }

//     OpenGLShader::~OpenGLShader()
//     {
//         Unbind();
//         glDeleteProgram(m_id);
//     }

//     void OpenGLShader::Bind() const
//     {
//         if (m_id != cur_bind_shader)
//         {
//             glUseProgram(m_id);
//             cur_bind_shader = m_id;
//         }
//     }

//     void OpenGLShader::Unbind() const
//     {
//         if (cur_bind_shader == m_id)
//         {
//             cur_bind_shader = 0;
//             glUseProgram(0);
//         }
//     }

//     void OpenGLShader::SetInt(const std::string& name, int value)
//     {
//         UploadUniformInt(name, value);
//     }

//     void OpenGLShader::SetIntArray(const std::string& name, const int* values, uint32_t count)
//     {
//         UploadUniformIntArray(name, values, count);
//     }

//     void OpenGLShader::SetFloat(const std::string& name, float value)
//     {
//         UploadUniformFloat(name, value);
//     }

//     void OpenGLShader::SetFloatArray(const std::string& name, const float* values, uint32_t count)
//     {
//         UploadUniformFloatArray(name, values, count);
//     }

//     void OpenGLShader::SetFloat2(const std::string& name, const glm::vec2& value)
//     {
//         UploadUniformFloat2(name, value);
//     }

//     void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
//     {
//         UploadUniformFloat3(name, value);
//     }
//     void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value)
//     {
//         UploadUniformFloat4(name, value);
//     }

//     void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)
//     {
//         UploadUniformMat4(name, value);
//     }

//     void OpenGLShader::UploadUniformInt(const std::string& name, int value)
//     {
//         GLint location = glGetUniformLocation(m_id, name.c_str());
//         glUniform1i(location, value);
//     }

//     void OpenGLShader::UploadUniformIntArray(const std::string& name, const int* values, uint32_t count)
//     {
//         GLint location = glGetUniformLocation(m_id, name.c_str());
//         glUniform1iv(location, count, values);
//     }

//     void OpenGLShader::UploadUniformFloat(const std::string& name, float value)
//     {
//         GLint location = glGetUniformLocation(m_id, name.c_str());
//         glUniform1f(location, value);
//     }

//     void OpenGLShader::UploadUniformFloatArray(const std::string& name, const float* values, uint32_t count)
//     {
//         GLint location = glGetUniformLocation(m_id, name.c_str());
//         glUniform1fv(location, count, values);
//     }

//     void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
//     {
//         GLint location = glGetUniformLocation(m_id, name.c_str());
//         glUniform2f(location, value.x, value.y);
//     }

//     void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
//     {
//         GLint location = glGetUniformLocation(m_id, name.c_str());
//         glUniform3f(location, value.x, value.y, value.z);
//     }

//     void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
//     {
//         GLint location = glGetUniformLocation(m_id, name.c_str());
//         glUniform4f(location, value.x, value.y, value.z, value.w);
//     }

//     void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
//     {
//         GLint location = glGetUniformLocation(m_id, name.c_str());
//         glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
//     }

//     void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
//     {
//         GLint location = glGetUniformLocation(m_id, name.c_str());
//         glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
//     }

//     std::string OpenGLShader::ReadFile(const std::string& filepath)
//     {
//         std::string result;
//         std::ifstream in(filepath, std::ios::in | std::ios::binary);
//         if (in)
//         {
//             in.seekg(0, std::ios::end);
//             size_t size = in.tellg();
//             if (size != -1)
//             {
//                 result.resize(size);
//                 in.seekg(0, std::ios::beg);
//                 in.read(&result[0], size);
//                 in.close();
//             }
//             else
//             {
//                 LOG_ERROR("Could not read from file '{0}", filepath);
//             }
//         }
//         else
//         {
//             LOG_ERROR("Coule not open file '{0}'", filepath);
//         }
//         return result;
//     }

//     void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
//     {

//         GLuint program = glCreateProgram();

//         // YUTREL_ASSERT(shaderSources.size() <= 2, "");

//         std::array<GLenum, 2> glShaderIDs;
//         int glShaderIDIndex = 0;
//         for (auto& kv : shaderSources)
//         {
//             GLenum type               = kv.first;
//             const std::string& source = kv.second;
//             GLuint shader             = glCreateShader(type);

//             // Send the vertex shader source code to GL
//             // Note that std::string's .c_str is NULL character terminated.
//             const GLchar* sourceCstr = source.c_str();
//             glShaderSource(shader, 1, &sourceCstr, 0);

//             // Compile the vertex shader
//             glCompileShader(shader);

//             GLint isCompiled = 0;
//             glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
//             if (isCompiled == GL_FALSE)
//             {
//                 GLint maxLength = 0;
//                 glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

//                 // The maxLength includes the NULL character
//                 std::vector<GLchar> infoLog(maxLength);
//                 glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

//                 // We don't need the shader anymore.
//                 glDeleteShader(shader);

//                 LOG_ERROR("{0}", infoLog.data());
//                 YUTREL_ASSERT(false, "Vertex shader compilation failure!");
//                 break;
//             }

//             // Attach our shaders to our program
//             glAttachShader(program, shader);
//             glShaderIDs[glShaderIDIndex++] = shader;
//         }

//         m_id = program;

//         // Link our program
//         glLinkProgram(program);
//         // Note the different functions here: glGetProgram* instead of glGetShader*.
//         GLint isLinked = 0;
//         glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
//         if (isLinked == GL_FALSE)
//         {
//             GLint maxLength = 0;
//             glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
//             // The maxLength includes the NULL character
//             std::vector<GLchar> infoLog(maxLength);
//             glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

//             // We don't need the program anymore.
//             glDeleteProgram(program);

//             for (auto id : glShaderIDs)
//             {
//                 glDeleteShader(id);
//             }

//             LOG_ERROR("{0}", infoLog.data());
//             YUTREL_ASSERT(false, "Shader link failure!");
//             return;
//         }

//         for (auto id : glShaderIDs)
//         {
//             glDetachShader(program, id);
//             glDeleteShader(id);
//         }
//     }

//     //////////////////////////////////////////////
//     // Compute Shader
//     //////////////////////////////////////////////
//     OpenGLComputeShader::OpenGLComputeShader(const std::string& shader_path)
//     {
//         std::string shader_src = ReadFile(shader_path);
//         Compile(shader_src);

//         // 从path中取得文件名，作为shader的名字
//         auto last_slash = shader_path.find_last_of("/\\");
//         last_slash      = last_slash == std::string::npos ? 0 : last_slash + 1;
//         auto lastDot    = shader_path.rfind('.');
//         auto count      = lastDot == std::string::npos ? shader_path.size() - last_slash : lastDot - last_slash;
//         m_name          = shader_path.substr(last_slash, count);
//     }

//     OpenGLComputeShader::~OpenGLComputeShader()
//     {
//         Unbind();
//         glDeleteProgram(m_id);
//     }

//     void OpenGLComputeShader::Bind() const
//     {
//         if (m_id != cur_bind_shader)
//         {
//             glUseProgram(m_id);
//             cur_bind_shader = m_id;
//         }
//     }

//     void OpenGLComputeShader::Unbind() const
//     {
//         if (cur_bind_shader == m_id)
//         {
//             cur_bind_shader = 0;
//             glUseProgram(0);
//         }
//     }

//     void OpenGLComputeShader::Dispatch(uint32_t nx, uint32_t ny, uint32_t nz) const
//     {
//         glDispatchCompute(nx, ny, nz);
//     }

//     void OpenGLComputeShader::SyncWait(GLbitfield barriers) const
//     {
//         glMemoryBarrier(barriers); // sync to ensure all writes are complete
//     }

//     void OpenGLComputeShader::SetInt(const std::string& name, int value)
//     {
//         UploadUniformInt(name, value);
//     }

//     void OpenGLComputeShader::SetIntArray(const std::string& name, int* values, uint32_t count)
//     {
//         UploadUniformIntArray(name, values, count);
//     }

//     void OpenGLComputeShader::SetFloat(const std::string& name, float value)
//     {
//         UploadUniformFloat(name, value);
//     }

//     void OpenGLComputeShader::SetFloat2(const std::string& name, const glm::vec2& value)
//     {
//         UploadUniformFloat2(name, value);
//     }

//     void OpenGLComputeShader::SetFloat3(const std::string& name, const glm::vec3& value)
//     {
//         UploadUniformFloat3(name, value);
//     }
//     void OpenGLComputeShader::SetFloat4(const std::string& name, const glm::vec4& value)
//     {
//         UploadUniformFloat4(name, value);
//     }

//     void OpenGLComputeShader::SetMat4(const std::string& name, const glm::mat4& value)
//     {
//         UploadUniformMat4(name, value);
//     }

//     void OpenGLComputeShader::UploadUniformInt(const std::string& name, int value)
//     {
//         GLint location = glGetUniformLocation(m_id, name.c_str());
//         glUniform1i(location, value);
//     }

//     void OpenGLComputeShader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count)
//     {
//         GLint location = glGetUniformLocation(m_id, name.c_str());
//         glUniform1iv(location, count, values);
//     }

//     void OpenGLComputeShader::UploadUniformFloat(const std::string& name, float value)
//     {
//         GLint location = glGetUniformLocation(m_id, name.c_str());
//         glUniform1f(location, value);
//     }

//     void OpenGLComputeShader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
//     {
//         GLint location = glGetUniformLocation(m_id, name.c_str());
//         glUniform2f(location, value.x, value.y);
//     }

//     void OpenGLComputeShader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
//     {
//         GLint location = glGetUniformLocation(m_id, name.c_str());
//         glUniform3f(location, value.x, value.y, value.z);
//     }

//     void OpenGLComputeShader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
//     {
//         GLint location = glGetUniformLocation(m_id, name.c_str());
//         glUniform4f(location, value.x, value.y, value.z, value.w);
//     }

//     void OpenGLComputeShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
//     {
//         GLint location = glGetUniformLocation(m_id, name.c_str());
//         glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
//     }

//     void OpenGLComputeShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
//     {
//         GLint location = glGetUniformLocation(m_id, name.c_str());
//         glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
//     }

//     std::string OpenGLComputeShader::ReadFile(const std::string& filepath)
//     {
//         std::string result;
//         std::ifstream in(filepath, std::ios::in | std::ios::binary);
//         if (in)
//         {
//             in.seekg(0, std::ios::end);
//             size_t size = in.tellg();
//             if (size != -1)
//             {
//                 result.resize(size);
//                 in.seekg(0, std::ios::beg);
//                 in.read(&result[0], size);
//                 in.close();
//             }
//             else
//             {
//                 LOG_ERROR("Could not read from file '{0}", filepath);
//             }
//         }
//         else
//         {
//             LOG_ERROR("Coule not open file '{0}'", filepath);
//         }
//         return result;
//     }

//     void OpenGLComputeShader::Compile(const std::string& shader_src)
//     {
//         // 创建着色器
//         auto shader = glCreateShader(GL_COMPUTE_SHADER);

//         // 把源码绑定到着色器上
//         const GLchar* sourceCstr = shader_src.c_str();
//         glShaderSource(shader, 1, &sourceCstr, NULL);

//         // 编译着色器
//         glCompileShader(shader);

//         // 检查编译是否成功
//         GLint isCompiled = 0;
//         glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
//         if (isCompiled == GL_FALSE)
//         {
//             GLint maxLength = 0;
//             glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

//             // The maxLength includes the NULL character
//             std::vector<GLchar> infoLog(maxLength);
//             glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

//             // We don't need the shader anymore.
//             glDeleteShader(shader);

//             LOG_ERROR("{0}", infoLog.data());
//             YUTREL_ASSERT(false, "Vertex shader compilation failure!");
//         }

//         // 创建程序
//         m_id = glCreateProgram();

//         // 把着色器绑定到程序上
//         glAttachShader(m_id, shader);

//         // 链接程序
//         glLinkProgram(m_id);

//         // 检查链接是否成功
//         GLint isLinked = 0;
//         glGetProgramiv(m_id, GL_LINK_STATUS, (int*)&isLinked);
//         if (isLinked == GL_FALSE)
//         {
//             GLint maxLength = 0;
//             glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &maxLength);
//             // The maxLength includes the NULL character
//             std::vector<GLchar> infoLog(maxLength);
//             glGetProgramInfoLog(m_id, maxLength, &maxLength, &infoLog[0]);

//             // We don't need the program anymore.
//             glDeleteProgram(m_id);

//             glDeleteShader(shader);

//             LOG_ERROR("{0}", infoLog.data());
//             YUTREL_ASSERT(false, "Shader link failure!");
//             return;
//         }

//         // 删除着色器
//         glDeleteShader(shader);
//     }

// } // namespace Yutrel