import pythonlab

from test_suite.scenario import Agros2DTestCase
from test_suite.scenario import Agros2DTestResult

from variant import ModelBase, ModelDict, ModelDictExternal
from test_suite.optilab.examples import quadratic_function
from shutil import copyfile

class TestModelDict(Agros2DTestCase):
    def setUp(self):
        self.md = ModelDict()
        self.md.directory = '{0}/models'.format(pythonlab.tempname())

    """
    def test_set_directory(self):
        model = ModelBase()
        self.md.add_model(model)

        with self.assertRaises(RuntimeError):
            self.md.directory = '{0}/models'.format(pythonlab.tempname())
    """

    def test_set_wrong_directory(self):
        with self.assertRaises(IOError):
            self.md.directory = '/models'

    def test_add_model(self):
        model = ModelBase()
        self.md.add_model(model)
        self.assertTrue(len(self.md.models))

    """
    def test_add_model_with_existing_name(self):
        model = ModelBase()
        self.md.add_model(model, 'model')
        self.md.save()
        self.md.clear()

        with self.assertRaises(KeyError):
            self.md.add_model(model, 'model')
    """

    """
    def test_add_model_with_automatic_name(self):
        model = ModelBase()
        indexes = [0, 1, 3]
        for index in indexes:
            self.md.add_model(model, name='model_{0:06d}'.format(index))
        self.md.save()

        self.md.add_model(model)
        self.md.save()
        self.md.clear()

        self.md.load(ModelBase)
        self.assertEqual(len(indexes)+1, len(self.md.models))

        self.md.load(ModelBase, mask='model_000002.pickle')
        self.assertEqual(len(indexes)+1, len(self.md.models))
    """

    def test_save_and_load(self):
        model = ModelBase()

        N = 10
        for i in range(N):
            self.md.add_model(model)
        self.md.save()

        self.md.clear()
        self.md.load(ModelBase)
        self.assertEqual(N, len(self.md.models))

    def test_load_files_with_wrong_directory(self):
        model = ModelBase()
        self.md.add_model(model)

        with self.assertRaises(RuntimeError):
            self.md.load(ModelBase, mask='{0}/models'.format(pythonlab.tempname()))

    def test_solve(self):
        variants = [(1, 2), (2, 3), (3, 4)]
        for a, x in variants:
            model = quadratic_function.QuadraticFunction()
            model.parameters['a'] = a
            model.parameters['x'] = x
            self.md.add_model(model)

        self.md.solve()
        for a, x in variants:
            model = self.md.find_model_by_parameters({'a' : a, 'x' : x})
            self.assertEqual(a*x**2, model.variables['F'])

    def test_solve_models_by_mask(self):
        for a, x in [(1, 2), (2, 3), (3, 4)]:
            model = quadratic_function.QuadraticFunction()
            model.parameters['a'] = a
            model.parameters['x'] = x
            self.md.add_model(model)

        self.md.solve(mask='model_00000[0, 2]')
        self.assertEqual(len(self.md.models), 3)
        self.assertEqual(len(self.md.solved_models), 2)

        for a, x in [(1, 2), (3, 4)]:
            model = self.md.find_model_by_parameters({'a' : a, 'x' : x})
            self.assertEqual(a*x**2, model.variables['F'])

class TestModelDictExternal(Agros2DTestCase):
    def setUp(self):
        self.md = ModelDictExternal()
        tmp = pythonlab.tempname()
        self.md.directory = '{0}/models'.format(tmp)
        self.md.solver = pythonlab.datadir('agros2d_solver')

        copyfile('{0}/resources/test/test_suite/optilab/examples/quadratic_function.py'.format(pythonlab.datadir()),
                 '{0}/problem.py'.format(tmp))

    def test_external_solver(self):
        model = quadratic_function.QuadraticFunction()
        model.parameters['a'] = 2
        model.parameters['x'] = 2

        self.md.add_model(model)
        self.md.save()

        self.md.solve()
        self.assertEqual(2*2**2, self.md.models[-1].variables['F'])

if __name__ == '__main__':
    import unittest as ut
    suite = ut.TestSuite()
    result = Agros2DTestResult()
    suite.addTest(ut.TestLoader().loadTestsFromTestCase(TestModelDict))
    suite.addTest(ut.TestLoader().loadTestsFromTestCase(TestModelDictExternal))
    suite.run(result)