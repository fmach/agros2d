import pythonlab

from test_suite.scenario import Agros2DTestCase
from test_suite.scenario import Agros2DTestResult

from variant import ModelBase

class Model(ModelBase):
    def declare(self):
        self.parameters.declare('u', list)
        self.parameters.declare('c', float, default=2.0)
        self.variables.declare('v', list)

    def solve(self):
        self.variables['v'] = []
        for ui in self.parameters['u']:
            self.variables['v'].append(ui * self.parameters['c'])

class TestModel(Agros2DTestCase):
    def test_undeclared_parameter(self):
        model = Model()
        with self.assertRaises(KeyError):
            model.parameters['undeclared_parameter'] = 1.0

    def test_undeclared_variable(self):
        model = Model()
        with self.assertRaises(KeyError):
            model.variables['undeclared_variable'] = 1.0

    def test_parameter_with_bad_data_type(self):
        model = Model()

        with self.assertRaises(TypeError):
            model.parameters['u'] = 1
        with self.assertRaises(TypeError):
            model.parameters['u'] = {'u1' : 3, 'u2' : 5}
        with self.assertRaises(TypeError):
            model.parameters['u'] = (3, 5)
        with self.assertRaises(TypeError):
            model.parameters['c'] = [3, 5]

        try:
            model.parameters['c'] = 1
            model.parameters['c'] = 1.0
        except:
            self.assertFalse(True, 'Exception raised')

    def test_default(self):
        model = Model()
        model.parameters['u'] = [5, 3, 6]
        model.solve()
        self.assertEqual([10, 6, 12], model.variables['v'])

    def test_rewrite_parameters(self):
        model = Model()
        with self.assertRaises(RuntimeError):
            model.parameters = {'p' : 0.0}

    def test_rewrite_variables(self):
        model = Model()
        with self.assertRaises(RuntimeError):
            model.variables = {'v' : 0.0}

    def test_rewrite_model_data(self):
        model = Model()
        with self.assertRaises(RuntimeError):
            model.data = dict()

    def test_save_and_load(self):
        file_name = pythonlab.tempname('.pickle')

        model = Model()
        model.parameters['u'] = [3, 5]
        model.parameters['c'] = 3
        model.solve()
        model.save(file_name)
        model.load(file_name)
        self.assertEqual([9, 15], model.variables['v'])

    def test_solved(self):
        model = Model()
        model.parameters['u'] = [1, 2, 3, 5]
        model.solve()
        self.assertTrue(model.solved)

        model.solved = False
        self.assertFalse(model.solved)
        self.assertTrue('v' in model.variables.data_types.keys())
        self.assertFalse('v' in model.variables.keys())

    def test_model_class(self):
        model = Model()
        self.assertEqual(Model, model.data.model_class)

    def test_serialize_and_deserialize(self):
        model = Model()
        model.parameters['u'] = [2, 3]
        model.parameters['c'] = 5
        model.solve()

        pickled_data = model.serialize()
        model.solved = False
        model.deserialize(pickled_data)
        self.assertTrue(model.solved)

if __name__ == '__main__':
    import unittest as ut
    suite = ut.TestSuite()
    result = Agros2DTestResult()
    suite.addTest(ut.TestLoader().loadTestsFromTestCase(TestModel))
    suite.run(result)