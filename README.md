# VP_merge

Программное обеспечение предназначенное для автоматизации выпуска текстовой конструкторской документации для проектов Altium Designer.



![MIT License](https://img.shields.io/badge/License-MIT-green.svg)![GitHub top language](https://img.shields.io/github/languages/top/work5lov/VP_merge)


## Входные данные
На вход программа принимает файлы с расширением .xml, генерируемые Board Assistant.
## Формат выходных файлов
Выходные файлы имеют формат .xlsx.
## Планы развития проекта

- Поддержка вывода данных в PDF
- Режим предпросмотра итогового файла в окне программы
- Добавление возможности генерации документации из Excel файла, состоящего из списка компонентов, получаемого при помощи OutputJob
- Переработка вывода данных в Excel-файл
- Сканирование папки с проектами для поиска xml файлов и упрощения их отображения
- Автоматическое обновление данных в окне предпросмотра при внесении изменений в проект (сработает лишь при работе с файлами из Board Assistant)
- Редактирование содержимого документации внутри режима предпросмотра