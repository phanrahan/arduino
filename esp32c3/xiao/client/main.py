from typing import Union
from pydantic import BaseModel
from fastapi import FastAPI
import datetime

app = FastAPI()
items = {}

class Sensor_Item(BaseModel):
    name: str
    value: float

@app.on_event("startup")
async def startup_event():
    items["sensor"] = {"name": "Sensor","Value":0}

@app.get("/items/{item_id}")
async def read_items(item_id: str):

    return items[item_id],datetime.datetime.now()

@app.post("/sensor/")
async def update_sensor(si: Sensor_Item):
    items["sensor"]["Value"] = si.value
    return si

@app.get("/")
def read_root():
    return {"Hello": "World"}
