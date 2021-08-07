#include "system/audioloader.h"

constexpr bool failed(const MMRESULT mr)
{
	return mr != MMSYSERR_NOERROR;
}

lycoris::system::audio::wav_file lycoris::system::audio::wav_loader::load_from_file(std::filesystem::path path)
{
	utility::xaudio2_voice<IXAudio2SourceVoice> voice;
	const auto mmio = mmioOpen(const_cast<char*>(path.string().c_str()), nullptr, MMIO_READ);
	MMRESULT mr;
	
	// riff chunk
	MMCKINFO riff;
	riff.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	{

		mr = mmioDescend(mmio, &riff, nullptr, MMIO_FINDRIFF);
		if (failed(mr))
		{
			mmioClose(mmio, MMIO_FHOPEN);
			throw std::runtime_error("WavLoader: failed to find riff chunk: " + path.string());
		}

	}
	
	// fmt chunk
	MMCKINFO fmt;
	WAVEFORMATEX format{};
	fmt.ckid = mmioFOURCC('f', 'm', 't', ' ');
	{
		mr = mmioDescend(mmio, &fmt, &riff, MMIO_FINDCHUNK);
		if (failed(mr))
		{
			mmioClose(mmio, MMIO_FHOPEN);
			throw std::runtime_error("WavLoader: failed to find fmt chunk: " + path.string());
		}
		
		if (mmioRead(mmio, reinterpret_cast<HPSTR>(&format), sizeof format) != sizeof format)
		{
			mmioClose(mmio, MMIO_FHOPEN);
			throw std::runtime_error("WavLoader: failed to load fmt chunk data: " + path.string());
		}

		mr = mmioAscend(mmio, &fmt, 0);
		if (failed(mr))
		{
			mmioClose(mmio, MMIO_FHOPEN);
			throw std::runtime_error("WavLoader: failed to exit fmt chunk: " + path.string());
		}

	}

	MMCKINFO data;
	data.ckid = mmioFOURCC('d', 'a', 't', 'a');
	
	mr = mmioDescend(mmio, &data, &riff, MMIO_FINDCHUNK);
	if (failed(mr))
	{
		mmioClose(mmio, MMIO_FHOPEN);
		throw std::runtime_error("WavLoader: failed to find data chunk: " + path.string());
	}
	std::vector<std::byte> buffer(data.cksize);
	if (mmioRead(mmio, reinterpret_cast<HPSTR>(buffer.data()), data.cksize) != data.cksize)
	{
		mmioClose(mmio, MMIO_FHOPEN);
		throw std::runtime_error("WavLoader: failed to load wav data: " + path.string());
	}
	
	mmioClose(mmio, MMIO_FHOPEN);
	return wav_file(format, std::move(buffer));
}
