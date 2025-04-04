import sys
from openpyxl import load_workbook
from openpyxl.worksheet.page import PageMargins

def set_page_margins(file_path):
    try:
        wb = load_workbook(file_path)        

        # Размеры страницы в миллиметрах
        margin_mm = 0.5  # 0.5 см

        # Преобразование миллиметров в дюймы
        mm_to_inches = 2.54
        margin_inches = margin_mm / mm_to_inches

        cm = margin_mm / mm_to_inches

        # Установка полей страницы
        for sheet_name in wb.sheetnames:
            # Проверяем, начинается ли имя листа с цифры 3
            ws = wb[sheet_name]  # Получаем объект листа
            ws.page_margins = PageMargins(left=cm, right=cm, top=cm, bottom=cm, footer=0, header=0)
            ws.print_options.horizontalCentered = True
            ws.print_options.verticalCentered = True

            ws.HeaderFooter.oddFooter.right.text = '&G&RФормат А3'
            ws.HeaderFooter.oddFooter.right.font = 'GOST type B'
            ws.HeaderFooter.oddFooter.right.size = 12
            ws.HeaderFooter.oddFooter.right.alignWithMargins = False
            ws.HeaderFooter.oddFooter.center.text = '&G&CКопировал'
            ws.HeaderFooter.oddFooter.center.font = 'GOST type B'
            ws.HeaderFooter.oddFooter.center.size = 12

            if sheet_name == 'Лист регистрации изменений':
                ws.page_setup.orientation = 'portrait'
                ws.page_setup.paperSize = ws.PAPERSIZE_A4
            else:
                ws.page_setup.orientation = 'landscape'
                ws.page_setup.paperSize = ws.PAPERSIZE_A3
            

        wb.save(file_path)
        print("Поля страницы установлены успешно.")
    except Exception as e:
        print(f"Произошла ошибка: {e}")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Использование: python script.py <путь_к_документу_excel>")
        sys.exit(1)
    
    excel_file = sys.argv[1]
    set_page_margins(excel_file)
