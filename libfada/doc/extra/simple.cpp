/***********************************************************
 ** libfada Full Example Visualizer
 **   Written by Nathan Cousins - December 2013
 ** 
 ** Usage: full_visualizer <audio_filepath>
 **   `audio_filepath' may be any filetype supported by SFML/libsndfile.
 ***********************************************************/


//////////////////////////////////////////////////
// Includes
//////////////////////////////////////////////////

#include <stdio.h>
#include <string>
#include <vector>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <fada/fada.hpp>

#if defined(_WIN32) || defined(__WIN32__)

#include <Windows.h>
#include <Shlwapi.h>

#ifndef _WINDOWS_
#define _WINDOWS_
#endif

#endif

//////////////////////////////////////////////////
// Declarations
//////////////////////////////////////////////////

class AudioAnalyzer;
class AudioFileStream;

sf::RenderWindow* rw = NULL;

AudioFileStream* audio = NULL;
sf::Time continueInterval, nextContinue;

bool isRunning;
sf::Mutex mutex;


const unsigned WINDOW_BUFFER_SIZE = 2048;
const unsigned FFT_BUFFER_SIZE    = WINDOW_BUFFER_SIZE * 2;
fada_Res       resultBuffer[WINDOW_BUFFER_SIZE];

sf::CircleShape orbBeat, orbBass;
sf::VertexArray vertexArray;


struct FreqBar {
	fada_Res value;
	double linePos, lineVel;
};

const unsigned NUM_FREQBARS = 12;
FreqBar        freqBars[NUM_FREQBARS];


enum {
	DRAW_NONE,
	DRAW_FFT,
	DRAW_WAVEFORM,
	DRAW_FFT_MIXED,
	DRAW_WAVEFORM_MIXED
} drawMode;

bool showSubBands;
bool showOrbs;
bool debugMode;

double    debugFPS, debugMS;
sf::Font* fontDebug;
sf::Text  txtDebug;

bool mouseHoverProgressBar;


// Application loop sub-routines.
bool initialize(const std::string& filename);
void finish();
void processEvents();
void tick(float ms);
void render();

// Utility functions.
fada_Res calcFreqBar(fada_Pos start, fada_Pos end);

// User events.
void onMouseDown(int x, int y, unsigned b);
void onMouseUp(int x, int y, unsigned b);
void onMouseWheel(int x, int y, int d);
void onMouseMove(int x, int y, int dx, int dy);
void onKeyDown(int key);
void onKeyUp(int key);
void onWindowResize(unsigned w, unsigned h);
void onWindowClose();

// Logging functions.
void logMessage(const char* func, const char* fmt, ...);
void logWarning(const char* func, const char* fmt, ...);
void logError(const char* func, const char* fmt, ...);

// FADA error checking.
fada_Error checkFADA(unsigned int line, fada_Error err);
#ifndef NDEBUG
#define CHECK_FADA(x) checkFADA(__LINE__, (x))
#else
#define CHECK_FADA(x) (x)
#endif


// Base class for audio analyzation via libfada.
class AudioAnalyzer
{
private:

	bool m_fadaLoaded;
	fada_Res m_beat, m_bass, m_normal;


protected:

	std::vector<fada_FFTBuffer*> m_buffers;
	fada_FFTBuffer* m_mixedBuffer;
	fada_Manager* m_fada;


	void setup(unsigned int sampleRate, unsigned int channelCount)
	{
		fada_Error err;
		m_fadaLoaded = false;

		// Set up our FADA manager.
		err = CHECK_FADA(fada_bindstream(m_fada, FADA_TSAMPLE_INT16, sampleRate, channelCount));
		if (err != FADA_ERROR_SUCCESS)
		{
			logError("AudioFileStream::onGetData", "could not initialize stream (manager=0x%p, rate=%d, channels=%d, error=%d)\n", m_fada, sampleRate, channelCount, err);
			return;
		}

		// Set up the analyzation window to 2048 frames.
		err = CHECK_FADA(fada_setwindowframes(m_fada, WINDOW_BUFFER_SIZE));
		if (err != FADA_ERROR_SUCCESS)
		{
			logWarning("AudioFileStream::onGetData", "could not set window size to 2048 (manager=0x%p, error=%d)\n", m_fada, err);
		}

		// Close any current buffers before making new ones.
		this->closeBuffers();

		// Set up FFT buffers.
		for (unsigned int i = 0; i < channelCount; ++i)
		{
			m_buffers.push_back(fada_newfftbuffer(FFT_BUFFER_SIZE));
		}
		m_mixedBuffer = fada_newfftbuffer(FFT_BUFFER_SIZE);

		m_normal = fada_getnormalizer(m_fada);
		m_fadaLoaded = true;
	}


	void closeBuffers()
	{
		// Close buffers.
		if (m_mixedBuffer)
			fada_closefftbuffer(m_mixedBuffer);
		m_mixedBuffer = NULL;

		for (size_t i = 0, sz = m_buffers.size(); i < sz; ++i)
			fada_closefftbuffer(m_buffers[i]);
		m_buffers.clear();
		
		// For added security, tell the manager that no FFT buffer is in use.
		CHECK_FADA(fada_usefftbuffer(m_fada, NULL));
	}


public:

	AudioAnalyzer()
	{
		m_fadaLoaded = false;
		m_beat = 0.;
		m_bass = 0.;
		m_normal = 1.;

		m_mixedBuffer = NULL;
		m_fada = fada_newmanager();
	}


	virtual ~AudioAnalyzer()
	{
		// Close our FFT buffers.
		this->closeBuffers();
		
		// Close manager.
		fada_closemanager(m_fada);
	}


	bool isReady() const
	{
		return m_fadaLoaded;
	}


	fada_Error pushSamples(const sf::Int16* samples, size_t sampleCount)
	{
		// Remove old sample data from the manager.
		fada_trimchunks(m_fada);

		// Push a new chunk of samples into the manager.
		return CHECK_FADA(fada_pushsamples(m_fada, (void*)samples, sampleCount, FADA_TRUE));
	}


	void update()
	{
		if (!m_fadaLoaded)
			return;

		// Calculate beat and audible bass.
		CHECK_FADA(fada_calcbeat(m_fada, &m_beat));
		m_beat /= m_normal;
		CHECK_FADA(fada_calcbass(m_fada, &m_bass));
		m_bass /= m_normal;
		
		// Generate a mixed FFT on the mixed buffer.
		CHECK_FADA(fada_usefftbuffer(m_fada, m_mixedBuffer));
		CHECK_FADA(fada_calcfft(m_fada));
		
		// Generate a channel-specific FFT on each buffer.
		for (size_t i = 0, sz = m_buffers.size(); i < sz; ++i)
		{
			CHECK_FADA(fada_usefftbuffer(m_fada, m_buffers[i]));
			CHECK_FADA(fada_calcfft_channel(m_fada, i));
		}
	}


	bool advance(long advanceFrames = FADA_NEXT_WINDOW)
	{
		return fada_continue(m_fada, advanceFrames) == FADA_TRUE;
	}


	fada_Res getBeat() const
	{
		return m_beat;
	}


	fada_Res getBass() const
	{
		return m_bass;
	}


	fada_Res getFFTValue(fada_Pos pos) const
	{
		// Get FFT value.
		fada_Res res;
		CHECK_FADA(fada_getfftvalue(m_fada, pos, &res));
		return res;
	}


	void getFFTValues(fada_Res* out, fada_Pos offset = 0, fada_Pos length = 0)
	{
		// Get all FFT values.
		if (offset == 0 && length == 0)
			CHECK_FADA(fada_getfftvalues(m_fada, out));
		// Get all `length' FFT values starting from offset.
		else
			CHECK_FADA(fada_getfftvaluesrange(m_fada, out, offset, length));
	}


	const fada_Res getNormal() const
	{
		return m_normal;
	}

	
	void useFFTBuffer(unsigned int channel)
	{
		if (channel >= m_buffers.size())
		{
			logWarning("AudioAnalyzer::useFFTBuffer", "invalid channel selected, must be below buffer count (manager=0x%p, channel=%d, buffers=%d)\n", m_fada, channel, m_buffers.size());
			return;
		}

		// Use this buffer for grabbing values from getFFTValue()
		CHECK_FADA(fada_usefftbuffer(m_fada, m_buffers[channel]));
	}


	void useFFTMixedBuffer()
	{
		// Use the mixed buffer for grabbing values from getFFTValue()
		CHECK_FADA(fada_usefftbuffer(m_fada, m_mixedBuffer));
	}


	fada_Manager* getManager()
	{
		return m_fada;
	}


	const fada_Manager* getManager() const
	{
		return m_fada;
	}

};


// Used for streaming an audio file, using SFML's Music class to stream sample chunks.
class AudioFileStream : public sf::Music, public AudioAnalyzer
{
protected:

	virtual void onSeek(sf::Time timeOffset)
	{
		//sf::Lock lock(mutex);
		sf::Music::onSeek(timeOffset);

		logMessage("AudioFileStream::onSeek", "seeking... (sec=%.2f)\n", timeOffset.asSeconds());
		fada_freechunks(m_fada);
	}


	virtual bool onGetData(sf::SoundStream::Chunk& chunk)
	{
		fada_Error err;

		if (sf::Music::onGetData(chunk))
		{
			
			sf::Lock lock(mutex);

			// Is this our first chunk?
			// Also make sure that audio information is still valid. This may change when changing audio sources.
			if (!this->isReady() ||
				fada_getchannels(m_fada) != this->getChannelCount() ||
				fada_getsamplerate(m_fada) != this->getSampleRate() )
			{
				this->setup(this->getSampleRate(), this->getChannelCount());
			}

			err = this->pushSamples(chunk.samples, chunk.sampleCount);
			if (err != FADA_ERROR_SUCCESS)
				logError("AudioFileStream::onGetData", "could not push samples (chunk=0x%p, count=%d, error=%d)\n", chunk.samples, chunk.sampleCount, err);

			return true;

		}

		return false;
	}

};


//////////////////////////////////////////////////
// Definitions
//////////////////////////////////////////////////


//////////////////////////////////////////////////
int main(int argc, char* args[])
{

	const float TICKRATE = 1.f/60;
	
	std::string filename = "";
	if (argc > 1)
		filename = args[1];
	
	isRunning = initialize(filename);
	if (!isRunning)
		return 1;

	sf::Clock clkFPS = sf::Clock();
	sf::Clock clkMS = sf::Clock();

	sf::Clock clk = sf::Clock();
	sf::Time clkTime;

	do
	{

		processEvents();

		while ((clkTime += clk.restart()) >= sf::seconds(TICKRATE))
		{
			clkMS.restart();

			clkTime -= sf::seconds(TICKRATE);
			tick(TICKRATE*1000.f);

			debugMS = clkMS.getElapsedTime().asMicroseconds() / 1000.;
		}

		render();

		debugFPS = 1000. / (clkFPS.restart().asMicroseconds() / 1000.);
		
		sf::sleep(sf::milliseconds(1));

	} while (isRunning);
	
	finish();
	return 0;

}


//////////////////////////////////////////////////
bool initialize(const std::string& filename)
{
	
	sf::Lock lock(mutex);

	std::string title = "Example libfada Visualizer";

#ifdef _WINDOWS_
	// Force application to use root executable directory on Windows.
	{
		char strPath[MAX_PATH];
		GetModuleFileName(NULL, strPath, MAX_PATH);
		std::string strExePathS = strPath;
		size_t found = strExePathS.find_last_of("/\\");
		strExePathS = strExePathS.substr(0,found);
		SetCurrentDirectory(strExePathS.c_str());

		title += " - ";
		if (PathCompactPathEx(strPath, filename.c_str(), 64, NULL))
		{
			title += strPath;
		}
		else
		{
			title += filename;
		}
	}
#else
	title += " - ";
	title += filename;
#endif

	// Create and open a new SFML window.
	rw = new sf::RenderWindow();
	rw->create(sf::VideoMode(800, 600), title, sf::Style::Default, sf::ContextSettings(16, 0, 4));

	// Setup VA for drawing the waveforms/transforms.
	vertexArray.resize(WINDOW_BUFFER_SIZE);
	vertexArray.setPrimitiveType(sf::LinesStrip);

	// Corner beat/bass orbs.
	orbBeat = sf::CircleShape(1.f, 60u);
	orbBass = sf::CircleShape(1.f, 60u);
	orbBeat.setPosition(0.f, (float)rw->getSize().y);
	orbBass.setPosition((float)rw->getSize().x, (float)rw->getSize().y);
	orbBeat.setOrigin(1.f, 1.f);
	orbBass.setOrigin(1.f, 1.f);
	orbBeat.setFillColor(sf::Color(255, 180, 0, 200));
	orbBass.setFillColor(sf::Color(0, 180, 255, 200));

	// Frequency subbands.
	for (unsigned i = 0; i < NUM_FREQBARS; ++i)
	{
		freqBars[i].value = 0.;
		freqBars[i].linePos = 0.;
		freqBars[i].lineVel = 0.;
	}

	// Set up font so we can render text.
	fontDebug = new sf::Font();
	if (!fontDebug->loadFromFile("font.ttf"))
		logWarning("initialize", "unable to load font file (filename=\"%s\"), file may be corrupted, unreadable, an invalid format, or not exist\n", "font.ttf");

	// Set up debug text.
	txtDebug.setFont(*fontDebug);
	txtDebug.setPosition(10.f, 10.f);
	txtDebug.setCharacterSize(14);

	// Let SFML decode the audio file if it isn't an mp3.
	// SFML will use streaming to load the audio.
	audio = new AudioFileStream();
	if (!audio->openFromFile(filename))
		return false;

	// Update every 1024 samples.
	continueInterval = sf::seconds(1024.f / audio->getSampleRate());
	nextContinue = continueInterval;

	// Initialize everything else.
	showSubBands = true;
	showOrbs = true;
	drawMode = DRAW_FFT;
	debugMode = false;
	
	mouseHoverProgressBar = false;

	// Some controls information.
	printf("libfada - Example Visualizer\n");
	printf("Playing '%s'\n", filename.c_str());
	printf("Left-click on the progress bar at the bottom of the screen to seek.\n");
	printf("\n");
	printf("[0]\tNone\n");
	printf("[1]\tStereo FFT (if applicable)\n");
	printf("[2]\tStereo waveform (if applicable)\n");
	printf("[3]\tMixed FFT\n");
	printf("[4]\tMixed waveform\n");
	printf("\n");
	printf("[I]\tToggle subband bars\n");
	printf("[O]\tToggle orbs\n");
	printf("[~]\tToggle debug mode\n");
	printf("\n");

	// Start the audio.
	audio->play();

	return true;

}


//////////////////////////////////////////////////
void finish()
{
	
	sf::Lock lock(mutex);

	if (audio)
	{
		audio->stop();
		delete audio;
	}

	if (fontDebug)
		delete fontDebug;

	if (rw)
	{
		if (rw->isOpen())
			rw->close();
		delete rw;
	}

}


//////////////////////////////////////////////////
void processEvents()
{
	
	sf::Lock lock(mutex);

	sf::Event ev;
	while (rw->pollEvent(ev))
	switch(ev.type)
	{

	case sf::Event::Closed:
		onWindowClose();
		break;

	case sf::Event::Resized:
		onWindowResize(ev.size.width, ev.size.height);
		break;

	case sf::Event::MouseButtonPressed:
		onMouseDown(ev.mouseButton.x, ev.mouseButton.y, ev.mouseButton.button);
		break;

	case sf::Event::MouseButtonReleased:
		onMouseUp(ev.mouseButton.x, ev.mouseButton.y, ev.mouseButton.button);
		break;

	case sf::Event::MouseMoved:
		{
			static int px = ev.mouseMove.x, py = ev.mouseMove.y;
			onMouseMove(ev.mouseMove.x, ev.mouseMove.y, ev.mouseMove.x - px, ev.mouseMove.y - py);
			px = ev.mouseMove.x;
			py = ev.mouseMove.y;
		} break;

	case sf::Event::MouseWheelMoved:
		onMouseWheel(ev.mouseWheel.x, ev.mouseWheel.y, ev.mouseWheel.delta);
		break;

	case sf::Event::KeyPressed:
		onKeyDown(ev.key.code);
		break;

	case sf::Event::KeyReleased:
		onKeyUp(ev.key.code);
		break;

	}

}


//////////////////////////////////////////////////
void tick(float ms)
{
	
	sf::Lock lock(mutex);

	static bool updateReady = true;
	
	const float SCRW = static_cast<float>(rw->getSize().x);
	const float SCRH = static_cast<float>(rw->getSize().y);

	if (!audio->isReady())
		return;

	if (updateReady)
	{
		// Update FADA beat, bass and FFT.
		audio->update();

		// Update sub-band values.
		const fada_Res FACTOR = 1.0;
		audio->useFFTMixedBuffer();
		freqBars[11].value = calcFreqBar(0, 3) / FACTOR;       // 0 - 43 Hz
		freqBars[10].value = calcFreqBar(4, 7) / FACTOR;       // 43 - 86 Hz
		freqBars[9].value  = calcFreqBar(8, 15) / FACTOR;      // 86 - 167 Hz
		freqBars[8].value  = calcFreqBar(16, 31) / FACTOR;     // 167 - 344 Hz
		freqBars[7].value  = calcFreqBar(32, 63) / FACTOR;     // 344 - 689 Hz
		freqBars[6].value  = calcFreqBar(64, 127) / FACTOR;    // 689 - 1378 Hz
		freqBars[5].value  = calcFreqBar(128, 255) / FACTOR;   // 1378 - 2756 Hz
		freqBars[4].value  = calcFreqBar(256, 511) / FACTOR;   // 2756 - 5512 Hz
		freqBars[3].value  = calcFreqBar(512, 767) / FACTOR;   // 5512 - 8268 Hz
		freqBars[2].value  = calcFreqBar(768, 1023) / FACTOR;  // 8268 - 11025 Hz
		freqBars[1].value  = calcFreqBar(1024, 1535) / FACTOR; // 11025 - 16537 Hz
		freqBars[0].value  = calcFreqBar(1536, 2047) / FACTOR; // 16537 - 22050 Hz

		// Update orbs based on beat and bass values.
		orbBeat.setScale(static_cast<float>(audio->getBeat() * 7000.), static_cast<float>(audio->getBeat() * 7000.));
		orbBass.setScale(static_cast<float>(audio->getBass() * 700.), static_cast<float>(audio->getBass() * 700.));

		updateReady = false;
	}

	// Update sub-bands.
	for (unsigned i = 0; i < NUM_FREQBARS; ++i)
	{
		FreqBar& bar = freqBars[i];

		bar.lineVel += 0.0005;
		bar.linePos -= bar.lineVel;
		if (bar.linePos < bar.value)
		{
			bar.linePos = bar.value;
			bar.lineVel = 0.;
		}
		if (bar.linePos < 0.)
		{
			bar.linePos = 0.;
			bar.lineVel = 0.;
		}
	}

	// Move FADA to the next window when audio has finished playing the current set of 1024 samples.
	while (audio->getPlayingOffset() >= nextContinue)
	{
		nextContinue += continueInterval;
		audio->advance(1024);
		updateReady = true;
	}

	// Stop program when audio is finished.
	if (audio->getStatus() == sf::Music::Stopped)
		isRunning = false;

}


//////////////////////////////////////////////////
void render()
{
	
	sf::Lock lock(mutex);

	if (!audio->isReady())
		return;
	
	const float SCRW   = static_cast<float>(rw->getSize().x);
	const float SCRH   = static_cast<float>(rw->getSize().y);
	const float SCRW_H = SCRW / 2.f;
	const float SCRH_H = SCRH / 2.f;

	const unsigned  MAX_CHANNEL_COLOURS = 8;
	const sf::Color CHANNEL_COLOURS[MAX_CHANNEL_COLOURS] = {
		sf::Color(200,  80,  80),
		sf::Color( 80,  80, 200),
		sf::Color( 80, 200,  80),
		sf::Color(200,  80, 200),
		sf::Color(200, 200,  80),
		sf::Color( 80, 200, 200),
		sf::Color( 80,  80,  80),
		sf::Color(200, 200, 200)
	};

	rw->clear();


	// Draw subband bars.
	if (showSubBands)
	{

		for (unsigned i = 0; i < NUM_FREQBARS; ++i)
		{

			sf::RectangleShape r;
			FreqBar& bar = freqBars[i];

			double p = bar.linePos * 3.;
			p = (p < 0.) ? 0. : (p > 2.) ? 2. : p;

			unsigned char red, green;
			red   = (p >= 1.) ? 255 : static_cast<unsigned char>(floor(    p  * 255.));
			green = (p <= 1.) ? 255 : static_cast<unsigned char>(floor((1.-p) * 255.));

			float x, width;
			width = (SCRW_H / NUM_FREQBARS) - 10.f;
			x = (width + 10.f) * i + 10.f + (SCRW_H / 2.f);

			// Draw bar.
			r.setSize(sf::Vector2f(width, -static_cast<float>(bar.value * SCRH / 2.) ));
			r.setPosition(x, SCRH + 16.f);
			r.setFillColor(sf::Color(red, green, 40, 160));
			rw->draw(r);

			p = bar.linePos * SCRH / 2.;
			p = (p < 12.) ? 12. : p;
			
			// Draw falling bar.
			r.setSize(sf::Vector2f( (SCRW_H / NUM_FREQBARS) - 10.f, 12.f) );
			r.setPosition(x, -static_cast<float>(p) + SCRH);
			r.setFillColor(sf::Color(255, 255, 255, 160));
			rw->draw(r);

		}

	}

	// Draw beat and bass orbs.
	if (showOrbs)
	{
		rw->draw(orbBeat);
		rw->draw(orbBass);
	}

	// Draw progress bar.
	{

		float progress = audio->getPlayingOffset().asSeconds() / audio->getDuration().asSeconds();
		
		sf::RectangleShape r;
		r.setFillColor(sf::Color(80, 80, 255));
		r.setPosition(0.f, SCRH - 6.f);
		r.setSize(sf::Vector2f(progress * SCRW, 6.f));
		rw->draw(r, sf::RenderStates(sf::BlendAdd));

		// Mouse-over highlight.
		if (mouseHoverProgressBar)
		{

			sf::VertexArray va;
			va.setPrimitiveType(sf::Quads);
			va.resize(4);

			va[0].position.x = 0.f;
			va[0].position.y = SCRH;
			va[1].position.x = SCRW;
			va[1].position.y = SCRH;
			va[2].position.x = va[1].position.x;
			va[2].position.y = va[1].position.y - 16.f;
			va[3].position.x = va[0].position.x;
			va[3].position.y = va[0].position.y - 16.f;

			va[0].color = sf::Color(200, 200, 200);
			va[1].color = va[0].color;
			va[2].color = sf::Color::Transparent;
			va[3].color = va[2].color;

			rw->draw(va, sf::RenderStates(sf::BlendAdd));

		}

	}

	// Draw FFT/Waveform.
	if (drawMode == DRAW_FFT)
	{

		// Draw FFTs for each channel.
		for (size_t c = 0, chans = audio->getChannelCount(); c < chans && c < MAX_CHANNEL_COLOURS; ++c)
		{

			audio->useFFTBuffer(c);
			audio->getFFTValues(resultBuffer, 0, WINDOW_BUFFER_SIZE);
			
			for (size_t i = 0, sz = vertexArray.getVertexCount(); i < sz && i < FFT_BUFFER_SIZE; ++i)
			{
				sf::Vertex& v = vertexArray[i];
				v.color = CHANNEL_COLOURS[c];
				v.position.x = i / static_cast<float>(sz) * SCRW;
				v.position.y = SCRH_H - static_cast<float>(resultBuffer[i] * SCRH * 10);
			}

			rw->draw(vertexArray, sf::RenderStates(sf::BlendAdd));
				
			// Mirror Y coodinates.
			for (size_t i = 0, sz = vertexArray.getVertexCount(); i < sz && i < FFT_BUFFER_SIZE; ++i)
			{
				sf::Vertex& v = vertexArray[i];
				v.position.y = SCRH - v.position.y;
			}

			rw->draw(vertexArray, sf::RenderStates(sf::BlendAdd));
			
		}
			
	}
	else if (drawMode == DRAW_FFT_MIXED)
	{

		// Draw the mixed-channel FFT.
		audio->useFFTMixedBuffer();
		audio->getFFTValues(resultBuffer, 0, WINDOW_BUFFER_SIZE);
			
		const sf::Color mixedColour = sf::Color(100, 220, 220);

		for (size_t i = 0, sz = vertexArray.getVertexCount(); i < sz && i < FFT_BUFFER_SIZE; ++i)
		{
			sf::Vertex& v = vertexArray[i];
			v.color = mixedColour;
			v.position.x = i / static_cast<float>(sz) * SCRW;
			v.position.y = SCRH_H - static_cast<float>(resultBuffer[i] * SCRH * 10);
		}

		rw->draw(vertexArray, sf::RenderStates(sf::BlendAdd));
			
		// Mirror Y coodinates.
		for (size_t i = 0, sz = vertexArray.getVertexCount(); i < sz; ++i)
		{
			sf::Vertex& v = vertexArray[i];
			v.position.y = SCRH - v.position.y;
		}

		rw->draw(vertexArray, sf::RenderStates(sf::BlendAdd));

	}
	else if (drawMode == DRAW_WAVEFORM)
	{

		// Draw waveforms for each channel.
		for (size_t c = 0, chans = audio->getChannelCount(); c < chans && c < MAX_CHANNEL_COLOURS; ++c)
		{

			CHECK_FADA(fada_getsamples(audio->getManager(), c, resultBuffer));

			for (size_t i = 0, sz = vertexArray.getVertexCount(); i < sz; ++i)
			{
				sf::Vertex& v = vertexArray[i];
				v.color = CHANNEL_COLOURS[c];
				v.position.x = i / static_cast<float>(sz) * SCRW;
				v.position.y = SCRH_H + static_cast<float>(resultBuffer[i] / audio->getNormal()) * (SCRH / 3.f);
			}

			rw->draw(vertexArray, sf::RenderStates(sf::BlendAdd));

		}

	}
	else if (drawMode == DRAW_WAVEFORM_MIXED)
	{

		CHECK_FADA(fada_getframes(audio->getManager(), resultBuffer));

		const sf::Color mixedColour = sf::Color(100, 220, 220);

		// Mixed channels.
		for (size_t i = 0, sz = vertexArray.getVertexCount(); i < sz; ++i)
		{
			sf::Vertex& v = vertexArray[i];
			v.color = mixedColour;
			v.position.x = i / static_cast<float>(sz) * SCRW;
			v.position.y = SCRH_H + static_cast<float>(resultBuffer[i] / audio->getNormal()) * (SCRH / 3.f);
		}

		rw->draw(vertexArray, sf::RenderStates(sf::BlendAdd));

	}

	// Draw debug to display FPS and tick MS.
	if (debugMode)
	{
		char buf[32];
		sprintf(buf, "FPS: %.0f\nMS:  %.2f", debugFPS, debugMS);
		txtDebug.setString(buf);
		
		txtDebug.setColor(sf::Color::Black);
		txtDebug.move(1.5f, 1.5f);
		rw->draw(txtDebug);
		txtDebug.move(-1.5f, -1.5f);
		
		txtDebug.setColor(sf::Color::White);
		rw->draw(txtDebug);
	}


	rw->display();

}


//////////////////////////////////////////////////
fada_Res calcFreqBar(fada_Pos start, fada_Pos end)
{

	const fada_Manager* fada = audio->getManager();
	fada_Pos fft_sz = fada_getfftsize(fada);

	if (start >= fft_sz)
		return 0.;

	if (end >= fft_sz)
		end = fft_sz-1;
	
	fada_Res cur;
	fada_Res res = 0.;
	for (unsigned i = start; i <= end; ++i)
	{
		CHECK_FADA(fada_getfftvalue(fada, i, &cur));
		res += cur;
	}

	return res;

}


//////////////////////////////////////////////////
void onMouseDown(int x, int y, unsigned b)
{

#ifdef _WINDOWS_
	// Weird SFML 2.1 bug on Windows - force window to focus when clicked on.
	::SetFocus(rw->getSystemHandle());
#endif

}


//////////////////////////////////////////////////
void onMouseUp(int x, int y, unsigned b)
{

	if (b != sf::Mouse::Left)
		return;
	
	// Audio seeking.
	if (mouseHoverProgressBar)
	{
		sf::Time offset = audio->getDuration() * (static_cast<float>(x) / rw->getSize().x);
		
		audio->setPlayingOffset(offset);
		nextContinue = offset;
	}

}


//////////////////////////////////////////////////
void onMouseWheel(int x, int y, int d)
{

}


//////////////////////////////////////////////////
void onMouseMove(int x, int y, int dx, int dy)
{

	mouseHoverProgressBar = ( static_cast<unsigned int>(y) > rw->getSize().y - 32 );

}


//////////////////////////////////////////////////
void onKeyDown(int key)
{

	switch (key)
	{
		// Debug mode.
		case sf::Keyboard::Tilde: debugMode = !debugMode; break;
			
		// Show subband bars.
		case sf::Keyboard::I: showSubBands = !showSubBands; break;

		// Show orbs.
		case sf::Keyboard::O: showOrbs = !showOrbs; break;

		// Draw modes.
		case sf::Keyboard::Num0:
		case sf::Keyboard::Numpad0:
			drawMode = DRAW_NONE;
			break;
		case sf::Keyboard::Num1:
		case sf::Keyboard::Numpad1:
			drawMode = DRAW_FFT;
			break;
		case sf::Keyboard::Num2:
		case sf::Keyboard::Numpad2:
			drawMode = DRAW_WAVEFORM;
			break;
		case sf::Keyboard::Num3:
		case sf::Keyboard::Numpad3:
			drawMode = DRAW_FFT_MIXED;
			break;
		case sf::Keyboard::Num4:
		case sf::Keyboard::Numpad4:
			drawMode = DRAW_WAVEFORM_MIXED;
			break;
	}

}


//////////////////////////////////////////////////
void onKeyUp(int key)
{

}


//////////////////////////////////////////////////
void onWindowResize(unsigned w, unsigned h)
{

	// Modify view to fill the new screen size.
	sf::View view = rw->getView();
	view.setSize(static_cast<float>(w), static_cast<float>(h));
	view.setCenter(w/2.f, h/2.f);
	rw->setView(view);

	// Reset positions of orbs.
	orbBeat.setPosition(0.f, static_cast<float>(rw->getSize().y));
	orbBass.setPosition(static_cast<float>(rw->getSize().x), static_cast<float>(rw->getSize().y));

}


//////////////////////////////////////////////////
void onWindowClose()
{

	isRunning = false;

}


//////////////////////////////////////////////////
void logMessage(const char* where, const char* fmt, ...)
{

	va_list args;
	va_start(args, fmt);

	printf("[INFO]    (in '%s'): ", where);
	vprintf(fmt, args);

	va_end(args);

}


//////////////////////////////////////////////////
void logWarning(const char* where, const char* fmt, ...)
{

	va_list args;
	va_start(args, fmt);

	printf("[WARNING] (in '%s'): ", where);
	vprintf(fmt, args);

	va_end(args);

}


//////////////////////////////////////////////////
void logError(const char* where, const char* fmt, ...)
{
	
	va_list args;
	va_start(args, fmt);

	printf("[ERROR]   (in '%s'): ", where);
	vprintf(fmt, args);

	va_end(args);

}


//////////////////////////////////////////////////
fada_Error checkFADA(unsigned int line, fada_Error err)
{

	if (err != FADA_ERROR_SUCCESS)
	{
		char buf[16];
		char* msg;

		switch (err)
		{
		default: msg = "unknown error"; break;
		case FADA_ERROR_INVALID_MANAGER:          msg = "passed an invalid manager"; break;
		case FADA_ERROR_INVALID_PARAMETER:        msg = "passed an invalid parameter"; break;
		case FADA_ERROR_INVALID_TYPE:             msg = "manager does not have a valid sample type bound to it"; break;
		case FADA_ERROR_INVALID_SIZE:             msg = "passed an invalid size"; break;
		case FADA_ERROR_INVALID_SAMPLE_RATE:      msg = "passed an invalid sample rate"; break;
		case FADA_ERROR_INVALID_CHANNEL:          msg = "passed an invalid channel index"; break;
		case FADA_ERROR_INVALID_FFT_BUFFER:       msg = "passed an invalid FFT buffer"; break;
		case FADA_ERROR_MANAGER_NOT_READY:        msg = "manager is not ready yet"; break;
		case FADA_ERROR_NO_DATA:                  msg = "no data was bound to the manager"; break;
		case FADA_ERROR_NOT_MULTIPLE_OF_CHANNELS: msg = "value was not a multiple of the channel count"; break;
		case FADA_ERROR_NOT_ENOUGH_MEMORY:        msg = "not enough memory"; break;
		case FADA_ERROR_INDEX_OUT_OF_BOUNDS:      msg = "index was out of bounds"; break;
		case FADA_ERROR_POSITION_OUT_OF_BOUNDS:   msg = "position was out of bounds"; break;
		case FADA_ERROR_FREQUENCY_OUT_OF_BOUNDS:  msg = "frequency was out of the valid frequency range"; break;
		case FADA_ERROR_WINDOW_NOT_CREATED:       msg = "manager does not have a window buffer created"; break;
		}

		sprintf(buf, "Line %d", line);
		logWarning(buf, "libfada check failed (error=%d): %s\n", err, msg);
	}

	return err;

}