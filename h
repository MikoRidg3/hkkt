import whisper
import ffmpeg
import os
from datetime import timedelta
from docx import Document

def mp4_to_wav(video_path: str) -> None:

    stream = ffmpeg.input(video_path)

    ffmpeg.run(ffmpeg.output(stream.audio, 'audio.wav', ar=16000, ac=1, acodec='pcm_s16le'))

def transcribe(audio_path: str) -> list:

    model = whisper.load_model('medium')

    result = model.transcribe(audio_path)
    os.remove(audio_path)
    segments = result['segments']

    text_massive = []
    prev_segment_end = ''
    for segment in segments:
        startTime = '0'+str(timedelta(seconds=int(segment['start'])))
        endTime = '0'+str(timedelta(seconds=int(segment['end'])))

        text = segment['text']
        segmentId = segment['id']+1
        duration = segment['end']-segment['start']

        text_without_punc = text
        for e in ''' ,.!?'"''':
            text_without_punc = text_without_punc.replace(e, '')
        letters = len(text_without_punc)

        line = f"[{segmentId} | {startTime} -> {endTime}]\n{text[1:] if text[0] == ' ' else text}\navg_letters_per_minute: {(letters/duration)*60:.2f}\npause: {segment['start']-prev_segment_end if prev_segment_end else ''}"
        text_massive.append(line)

        prev_segment_end = segment['end']

    return text_massive

def to_txt(text_massive: list) -> None:

    with open('result.txt', 'w', encoding='utf-8') as f:
       f.write('\n'.join(text_massive))


def main():
    mp4_to_wav('Питчинг.mp4')
    to_txt(transcribe('audio.wav'))
    #print(*transcribe('audio.wav'), sep='\n')s

if __name__ == '__main__':
    main()
