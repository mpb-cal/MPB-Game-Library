#ifndef __MPBTEXTURE_H__
#define __MPBTEXTURE_H__


class MPBTexture
{
public:

	// fileName must be a 256-color bitmap file
	MPBTexture::MPBTexture( const char* name, BYTE alpha, bool mipmap, GLint wrapType );
	MPBTexture::~MPBTexture();

	void loadFromBMPFile( char* filename );
	void loadFromBMPAlphaFile( char* filename );
	void loadFromFile( char* filename );
	void loadFromMemory(	BYTE* reds,
								BYTE* greens,
								BYTE* blues,
								BYTE* alphas,
								int width,
								int height,
								int depth	);
	void loadFromMemoryPixels(	BYTE* pixels,
										int width,
										int height,
										int depth	);
	void bind();
	char* getName() { return m_name; }
	GLenum getTexType() { return m_texType; }

	bool			m_used;

private:

	char			m_name[MAX_PATH];
	bool			m_mipmap;
	GLenum		m_texType;
	GLuint		m_textureID;
	BYTE			m_alpha;
	GLint			m_wrapType;
};


#endif

