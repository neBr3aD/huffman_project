import os
import asyncio
import sys
from pathlib import Path
from my_token_bot import TOKEN
from aiogram import Bot, Dispatcher, F
from aiogram.types import Message, FSInputFile
from aiogram.filters import CommandStart

PROJECT_DIR = Path(__file__).resolve().parent
sys.path.insert(0, str(PROJECT_DIR / "build"))

import huffman_archiver

bot = Bot(token=TOKEN)
dp = Dispatcher()

@dp.message(CommandStart())
async def start_cmd(message: Message):
    await message.answer(
        "Привет! Я бот-архиватор.\n\n"
        "📁 Скинь мне `.txt` файл, и я сожму его своим алгоритмом Хаффмана.\n"
        "📦 Скинь мне архив `.huff`, и я распакую его обратно в текст."
    )

@dp.message(F.document)
async def handle_document(message: Message):
    doc = message.document
    file_name = doc.file_name

    if not (file_name.endswith('.txt') or file_name.endswith('.huff')):
        await message.answer("Я принимаю только текстовые файлы (.txt) или мои архивы (.huff).")
        return

    status_msg = await message.answer("📥 Скачиваю файл...")

    file_info = await bot.get_file(doc.file_id)
    await bot.download_file(file_info.file_path, destination=file_name)

    try:
        if file_name.endswith('.txt'):
            await status_msg.edit_text("⚙️ Сжимаю файл...")
            out_name = file_name.replace('.txt', '.huff')

            huffman_archiver.compress(file_name, out_name)

            await status_msg.edit_text("📤 Отправляю архив...")
            archive = FSInputFile(out_name)
            await message.answer_document(archive, caption="✅ Готово! Файл успешно сжат.")
            os.remove(out_name)

        elif file_name.endswith('.huff'):
            await status_msg.edit_text("⚙️ Распаковываю архив...")
            out_name = file_name.replace('.huff', '_restored.txt')

            huffman_archiver.decompress(file_name, out_name)

            await status_msg.edit_text("📤 Отправляю распакованный текст...")
            restored = FSInputFile(out_name)
            await message.answer_document(restored, caption="✅ Готово! Архив успешно распакован.")
            os.remove(out_name)

    except Exception as e:
        await status_msg.edit_text(f"❌ Произошла ошибка на стороне C++: {e}")
    finally:

        if os.path.exists(file_name):
            os.remove(file_name)

async def main():
    print("Бот запущен. Нажмите Ctrl+C для остановки.")
    await dp.start_polling(bot)

if __name__ == "__main__":
    asyncio.run(main())