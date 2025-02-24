#!/usr/bin/env python
# coding: utf-8

import json 
import os
import uuid
import xml.etree.ElementTree as ET
import yaml
from PIL import Image


#Необходимо отредактировать в соответствии с действительным местоположением базы данных, аннотированной в формате YOLO
path_yolo = "C:\\Users\\olgas\\Desktop\\FlowersTypes.v5i.yolov8\\"


def create_dict_xml(path, classes):

    # Создаем dictionary.xml
    root = ET.Element("TLFSemanticDictinary", Description="")

    # Создаем элемент TLFSemanticDictinaryItem с атрибутами
    for class_name, class_id in classes.items():
        item = ET.SubElement(root, "TLFSemanticDictinaryItem", {
            "noun": class_name,
            "color": "32768",
            "id": class_id,
            "zoneType": "1"
        })

        # Создаем элемент ILFScanner внутри TLFSemanticDictinaryItem
        scanner = ET.SubElement(item, "ILFScanner", {
            "type": "TLFScanner",
            "ApertureWidth": "24",
            "ApertureHeight": "24"
        })

    # Преобразуем дерево в строку
    xml_data = ET.tostring(root, encoding='utf-8', method='xml').decode()

    # Добавляем заголовок XML
    xml_data = '<?xml version="1.0" ?>\n' + xml_data

    # Сохраняем в файл
    with open(path, "w", encoding="utf-8") as file:
        file.write(xml_data)
        
def read_yolo_annotation(file_path, classes_names):
    annotations = []
    with open(file_path, 'r') as file:
        lines = file.readlines()
        for line in lines:
            # Split the line into components
            values = line.strip().split()
            # Convert the values to float (for bbox) and int (for class_id)
            class_id = int(values[0])
            x_center = float(values[1])
            y_center = float(values[2])
            width = float(values[3])
            height = float(values[4])
            # Add the parsed annotation to the list
            annotations.append([classes_names[class_id], x_center, y_center, width, height])
    return annotations

def create_semantic_descr_from_yolo(img, img_filename, classes, anno_list):
    root = ET.Element("TLFSemanticImageDescriptor", ImageWidth = str(img.size[0]), ImageHeight = str(img.size[1]))
    for name, x_center, y_center, width, height in anno_list:
        item = ET.SubElement(root, "TLFDetectedItem", {
                "DetectorName": "Human marked",
                "Raiting": "0",
                "Type": classes[name],
                "zoneType": "1",
                "Angle": "0",
                "Racurs": "0",
                "Bw": "24",
                "Bh": "24",
                "left": str(int(x_center*img.size[0] - width*img.size[0]/2)),
                "top": str(int(y_center*img.size[1] - height*img.size[1]/2)),
                "right": str(int(x_center*img.size[0] + width*img.size[0]/2)),
                "bottom": str(int(y_center*img.size[1] + height*img.size[1]/2)),
                "Comment": ""
            })
    # Преобразуем дерево в строку
    xml_data = ET.tostring(root, encoding='utf-8', method='xml').decode()

    # Добавляем заголовок XML
    xml_data = '<?xml version="1.0" ?>\n' + xml_data

    # Сохраняем в файл
    with open(path_yolo + "valid\\images\\" + img_filename[:-3] + "xml", "w", encoding="utf-8") as file:
        file.write(xml_data)


with open(path_yolo + 'data.yaml', 'r') as file:
    data = yaml.safe_load(file)

classes_yolo = {x: str(uuid.uuid4()) for x in data["names"]}
classes_names = data["names"]
path4dict = path_yolo + "valid\\images\\dictionary.xml"
create_dict_xml(path4dict, classes_yolo)

folder_path = path_yolo + "valid\\images\\"
anno_folder_path = path_yolo + "valid\\labels\\"
for filename in os.listdir(folder_path):
    if filename.endswith(('.jpg', '.jpeg', '.png')):
        img_path = os.path.join(folder_path, filename)
        # Open the image using PIL
        img = Image.open(img_path)
        anno_path = os.path.join(anno_folder_path, filename[:-3] + "txt")
        annotation = read_yolo_annotation(anno_path, classes_names)
        create_semantic_descr_from_yolo(img, filename, classes_yolo, annotation)

