
#include "MPBGlobal.h"
#include "MPBMessageWindow.h"
#include "MPBOpenGL.h"
#include "MPBTexture.h"
#include "mmgr.h"

#define TRANSPARENT_COLOR		255 

// alpha is the amount of opacity to give this texture (0 - 255)
MPBTexture::MPBTexture( const char* name, BYTE alpha, bool mipmap, GLint wrapType ) :
	m_mipmap( mipmap ),
	m_alpha( alpha ),
	m_used( false ),
	m_wrapType( wrapType )
{
	strcpy( m_name, name );
}

MPBTexture::~MPBTexture()
{
}

void MPBTexture::loadFromBMPFile( char* filename )
{
	sprintf( g_logBuf, "loading texture %s", filename );
//	MPBMessageWindow::writeMessage( g_logBuf );

	BYTE* reds;
	BYTE* greens;
	BYTE* blues;
	int width;
	int height;

	readBitmapColorValues( filename, reds, greens, blues, width, height );
	int pixelCount = width * height;
//	MPBMessageWindow::writeMessage( "read color values" );

	loadFromMemory(	reds,
							greens,
							blues,
							NULL,
							width,
							height,
							1	);

	NULLIFY_ARRAY( reds );
	NULLIFY_ARRAY( greens );
	NULLIFY_ARRAY( blues );

}

void MPBTexture::loadFromBMPAlphaFile( char* filename )
{
	sprintf( g_logBuf, "loading texture %s", filename );

	int width;
	int height;

	BYTE* colors = readBitmapColors( filename, width, height );
	int pixelCount = width * height;

	BYTE* reds = new BYTE[ pixelCount ];
	BYTE* greens = new BYTE[ pixelCount ];
	BYTE* blues = new BYTE[ pixelCount ];
	BYTE* alphas = new BYTE[ pixelCount ];

	for (int i=0; i<pixelCount; i++)
	{
		reds[i] = 255;
		greens[i] = 255;
		blues[i] = 255;
		alphas[i] = colors[i];
	}

	loadFromMemory(	reds,
							greens,
							blues,
							alphas,
							width,
							height,
							1	);

	NULLIFY_ARRAY( colors );
	NULLIFY_ARRAY( reds );
	NULLIFY_ARRAY( greens );
	NULLIFY_ARRAY( blues );
	NULLIFY_ARRAY( alphas );

}

void MPBTexture::loadFromFile( char* texFilename )
{
	char buf[1000];

	sprintf( g_logBuf, "loading texture %s", texFilename );
	MPBMessageWindow::writeMessage( g_logBuf );

	int width;
	int height;

	FILE* fp = fopen( texFilename, "rb" );
	fread( &width, sizeof( int ), 1, fp );
	fread( &height, sizeof( int ), 1, fp );

	BYTE* pixels = new BYTE[ width * height * 4 ];
	fread( pixels, sizeof( unsigned char ), width * height * 4, fp );

	fclose( fp );

	loadFromMemoryPixels( pixels, width, height, 1 );
	NULLIFY_ARRAY( pixels );
}

// 1D textures: height and depth = 1
// 2D textures: depth = 1
// if alphas is NULL, all alphas will be set to m_alpha, or 0 if red, green and blue are TRANSPARENT_COLOR
void MPBTexture::loadFromMemory(	BYTE* reds,
											BYTE* greens,
											BYTE* blues,
											BYTE* alphas,
											int width,
											int height,
											int depth	)
{
	MPB_ASSERT( width );
	MPB_ASSERT( height );
	MPB_ASSERT( depth );

	sprintf( g_logBuf, "loading texture from memory %s", m_name );
	MPBMessageWindow::writeMessage( g_logBuf );

	int pixelCount = width * height * depth;

	// convert bitmap's array of color indexes to array of RGBA (4 bytes each)
	// any pixels which are #FFFFFF will ba made transparent

	BYTE* pixels = new BYTE[ pixelCount * 4 ];

	int p = 0;
	int row, column, rank;

	p = 0;
	for (rank=0; rank<depth; rank++)
	{
		for (row=0; row<height; row++)
		{
			for (column=0; column<width; column++)
			{
				int index = width * height * rank + width * row + column;
				BYTE red = *(reds + index);
				BYTE green = *(greens + index);
				BYTE blue = *(blues + index);

				pixels[p * 4 + 0] = red;
				pixels[p * 4 + 1] = green;
				pixels[p * 4 + 2] = blue;

				if (alphas)
				{
					BYTE alpha = *(alphas + index);
					pixels[p * 4 + 3] = alpha;
				}
				else
				{
					pixels[p * 4 + 3] = m_alpha;

					if (	red == TRANSPARENT_COLOR &&
							green == TRANSPARENT_COLOR &&
							blue == TRANSPARENT_COLOR	)
					{
						pixels[p * 4 + 3] = 0;		// transparent
					}
				}

				p++;
			}
		}
	}

//	MPBMessageWindow::writeMessage( "converted to RGBA" );

	loadFromMemoryPixels( pixels, width, height, depth );
	NULLIFY_ARRAY( pixels );
}

// each pixel is 4 bytes: RGBA
void MPBTexture::loadFromMemoryPixels(	BYTE* pixels,
													int width,
													int height,
													int depth	)
{
	char buf[1000];

	if (depth == 1)
	{
		if (width == 1)
		{
			m_texType = GL_TEXTURE_1D;
		}
		else
		{
			m_texType = GL_TEXTURE_2D;
		}
	}
	else
	{
		m_texType = GL_TEXTURE_3D;
	}

	glEnable( m_texType );
	glGenTextures( 1, &m_textureID );
	glBindTexture( m_texType, m_textureID );

	if (m_texType == GL_TEXTURE_3D) m_mipmap = false;

	if (m_mipmap)
	{
		glTexParameteri( m_texType, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( m_texType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	}
	else
	{
		glTexParameteri( m_texType, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( m_texType, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	}

	glTexParameteri( m_texType, GL_TEXTURE_WRAP_R, m_wrapType );
	glTexParameteri( m_texType, GL_TEXTURE_WRAP_S, m_wrapType );
	glTexParameteri( m_texType, GL_TEXTURE_WRAP_T, m_wrapType );

	if (m_mipmap)
	{
		if (m_texType == GL_TEXTURE_1D)
		{
			gluBuild1DMipmaps(
				m_texType, 
				4, 
				width,
				GL_RGBA, 
				GL_UNSIGNED_BYTE,
				pixels	);
		}
		else if (m_texType == GL_TEXTURE_2D)
		{
			gluBuild2DMipmaps(
				m_texType, 
				4, 
				width,
				height, 
				GL_RGBA, 
				GL_UNSIGNED_BYTE,
				pixels	);
		}
	}
	else
	{
		if (m_texType == GL_TEXTURE_1D)
		{
			glTexImage1D(
				m_texType,					// target
				0,							// level of detail
				GL_RGBA,					// internal format
				width,					// width
				0,							// border
				GL_RGBA,					// format
				GL_UNSIGNED_BYTE,		// type
				pixels	);				// pixels
		}
		else if (m_texType == GL_TEXTURE_2D)
		{
			glTexImage2D(
				m_texType,					// target
				0,							// level of detail
				GL_RGBA,					// internal format
				width,					// width, must be 64+ and 2m + 2b, eg 66 if border=1
				height,					// height, must be 64+ and 2m + 2b
				0,							// border
				GL_RGBA,					// format
				GL_UNSIGNED_BYTE,		// type
				pixels	);				// pixels
		}
		else if (m_texType == GL_TEXTURE_3D)
		{
			MPBOpenGL::glTexImage3D(
				m_texType,					// target
				0,							// level of detail
				GL_RGBA,					// internal format
				width,					// width, must be 64+ and 2m + 2b, eg 66 if border=1
				height,					// height, must be 64+ and 2m + 2b
				depth,
				0,							// border
				GL_RGBA,					// format
				GL_UNSIGNED_BYTE,		// type
				pixels	);				// pixels
		}

		MPBOpenGL::errorCheck();
	}

	glDisable( m_texType );

	sprintf( buf, "Created texture: %s, id=%d\n", m_name, m_textureID );
	writeLog( buf );
//	MPBMessageWindow::writeMessage( "built mipmaps" );
}

void MPBTexture::bind()
{
/*	if (m_texType == GL_TEXTURE_1D)
		MPB_ASSERT( m_wrapType == GL_REPEAT );
	if (m_texType == GL_TEXTURE_2D)
		MPB_ASSERT( m_wrapType == GL_REPEAT );
	if (m_texType == GL_TEXTURE_3D)
		MPB_ASSERT( m_wrapType == GL_CLAMP_TO_EDGE );*/

//	glPushAttrib( GL_TEXTURE_BIT );

//	sprintf( g_logBuf, "binding texture %s, %d, %d\n", m_name, m_texType, m_wrapType );
//	writeLog();

	// mpb! this messes up particle system textures
//	glTexParameteri( m_texType, GL_TEXTURE_WRAP_R, m_wrapType );
//	glTexParameteri( m_texType, GL_TEXTURE_WRAP_S, m_wrapType );
//	glTexParameteri( m_texType, GL_TEXTURE_WRAP_T, m_wrapType );

	glBindTexture( m_texType, m_textureID );
//	MPBOpenGL::errorCheck();
}





