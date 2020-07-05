import automobile

class YamahaPhoto(automobile.Photograph):
    
    def __init__(self):
        super().__init__()
        
    def is_beautiful(self, bike):
        return True

bike = automobile.Motorcycle.create("yamaha")

photo = YamahaPhoto()
print(photo.is_beautiful(bike))