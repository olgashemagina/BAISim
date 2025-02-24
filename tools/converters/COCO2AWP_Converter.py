#!/usr/bin/env python
# coding: utf-8

from pycocotools.coco import COCO
import json 
import os
import uuid
import xml.etree.ElementTree as ET

# Изменить path в соответствии с реальным местоположением базы данных с аннотацией в формате COCO
path = 'C:\\Users\\olgas\\fiftyone\\coco-2017\\'

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

def create_semantic_descr(img, classes, anno_list):
    root = ET.Element("TLFSemanticImageDescriptor", ImageWidth = str(img["width"]), ImageHeight = str(img["height"]))
    for name, left, top, right, bottom in anno_list:
        item = ET.SubElement(root, "TLFDetectedItem", {
                "DetectorName": "Human marked",
                "Raiting": "0",
                "Type": classes[name],
                "zoneType": "1",
                "Angle": "0",
                "Racurs": "0",
                "Bw": "24",
                "Bh": "24",
                "left": str(left),
                "top": str(top),
                "right": str(right),
                "bottom": str(bottom),
                "Comment": ""
            })
    # Преобразуем дерево в строку
    xml_data = ET.tostring(root, encoding='utf-8', method='xml').decode()

    # Добавляем заголовок XML
    xml_data = '<?xml version="1.0" ?>\n' + xml_data

    # Сохраняем в файл
    with open(path + "validation\\data\\" + img["file_name"][:-3] + "xml", "w", encoding="utf-8") as file:
        file.write(xml_data)


coco = COCO(path + 'raw\\instances_val2017.json')
# Opening JSON file
f = open(path + 'info.json')

# returns JSON object as a dictionary
data = json.load(f)
classes = {x: str(uuid.uuid4()) for x in data["classes"]}
path4dict = path + "validation\\data\\dictionary.xml"
create_dict_xml(path4dict, classes)


imgIds = coco.getImgIds()

for imgId in imgIds:
    img = coco.loadImgs(imgId)[0]
    annIds = coco.getAnnIds(imgIds=imgId)
    anno_list = []
    for annId in annIds:
        ann = coco.loadAnns(annId)[0]
        cat = coco.loadCats(ann['category_id'])[0]

        if cat['name'] == 'background':
            continue

        box = ann['bbox']
        anno_list.append([cat['name'], int(box[0]), int(box[1]), int(box[0]+box[2]), int(box[1]+box[3])])
    create_semantic_descr(img, classes, anno_list)





