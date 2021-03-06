from variant import ModelBase

class BinhKornFunction(ModelBase):
    """ f1(x,y) = 4 * x**2 + 4 * y**2; f2(x,y) = (x - 5)**2 + (y - 5)**2 """
    def solve(self):
        try:
            self.F1 = 4 * self.parameters['x']**2 + 4 * self.parameters['y']**2
            self.F2 = (self.parameters['x'] - 5)**2 + (self.parameters['y'] - 5)**2

            self.solved = True
        except:
            self.solved = False

    def process(self):
        self.variables['F1'] = self.F1
        self.variables['F2'] = self.F2
