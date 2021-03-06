import pythonlab
import random as rnd

from test_suite.scenario import Agros2DTestCase
from test_suite.scenario import Agros2DTestResult

from test_suite.optilab.examples import quadratic_function
from test_suite.optilab.examples import booths_function

from variant import ModelBase, ModelDict

from variant.optimization import *
from variant.optimization.genetic import *

def test_population(genoms):
    parameters = Parameters([ContinuousParameter("a", -5, -1),
                             ContinuousParameter("b", 6, 8),
                             ContinuousParameter("c", 0, 99),
                             ContinuousParameter("d", 0, 1),
                             DiscreteParameter("e", [1, 3, 6, 88, 2]),
                             DiscreteParameter("f", [-41, 8, 1, 2, 3]),
                             DiscreteParameter("g", [21, 75, 25, 88, 11]),
                             DiscreteParameter("h", [85, 12, -147])])

    population_creator = ImplicitInitialPopulationCreator(ModelBase, parameters)
    return population_creator.create(genoms), parameters

class TestCrossover(Agros2DTestCase):
    def setUp(self):
        self.population, self.parameters = test_population(10)
        self.crossover_creator = ImplicitCrossover()

    def test_crossover(self):
        mother = rnd.choice(self.population)
        father = rnd.choice(self.population)
        son = self.crossover_creator.cross(mother, father)

        self.assertTrue(son.parameters != mother.parameters)
        self.assertTrue(son.parameters != father.parameters)

        for key, value in son.parameters.items():
            self.assertTrue((value == mother.parameters[key]) or (value == father.parameters[key]))

        mother_gens_count = 0
        father_gens_count = 0
        for key, value in son.parameters.items():
            if (mother.parameters[key] == value):
                mother_gens_count += 1
            else:
                father_gens_count += 1

        self.assertTrue((mother_gens_count != 0) or (father_gens_count != 0))

class TestMutation(Agros2DTestCase):
    def setUp(self):
        self.population, self.parameters = test_population(10)
        self.mutation_creator = ImplicitMutation(self.parameters)

    def test_mutation(self):
        original = rnd.choice(self.population)
        mutant = self.mutation_creator.mutate(original)

        mutations = 0
        for key in original.parameters.keys():
            if (original.parameters[key] != mutant.parameters[key]): mutations += 1

        self.assertTrue(mutations != 0)

class TestSingleCriteriaSelector(Agros2DTestCase):
    def setUp(self):
        md = ModelDict()
        variants = [1e-3, 1e-2, 1e2, 1e-4, 1e-1, 
                    1e-5, 1e-8, 1e3, 1e6, 1e-8]

        for x in variants:
            model = quadratic_function.QuadraticFunction()
            model.parameters['x'] = x
            GeneticInfo.set_population_from(model, 0)
            GeneticInfo.set_population_to(model, 0)
            md.add_model(model)

        md.solve(save=False)

        self.functionals = Functionals(Functional('F', 'min'))
        self.selector = SingleCriteriaSelector(self.functionals)
        self.selector.recomended_population_size = len(variants)
        self.population = md.models

    def test_selection(self):
        selected = self.selector.select(self.population)
        self.assertTrue(len(selected) < len(self.population))
        for genom in selected:
            self.assertTrue(self.functionals.evaluate(genom) < 1e-1)

    def test_priority(self):
        selected = self.selector.select(self.population)

        minimum = 1
        maximum = 0
        for genom in selected:
            score = self.functionals.evaluate(genom)
            if (score < minimum): minimum = score
            if (score > maximum): maximum = score

        for genom in selected:
            score = self.functionals.evaluate(genom)
            if (score == minimum):
                self.assertTrue(GeneticInfo.priority(genom) == 3)
            elif (score == maximum):
                self.assertTrue(GeneticInfo.priority(genom) == 1)
            else:
                self.assertTrue(GeneticInfo.priority(genom) == 2)

class TestBoothsFunctionOptimization(Agros2DTestCase):
    def test_optimization(self):
        parameters = Parameters([ContinuousParameter('x', -10, 10), ContinuousParameter('y', -10, 10)])
        functionals = Functionals([Functional("F", "min")])

        optimization = GeneticOptimization(parameters, functionals,
                                           booths_function.BoothsFunction)

        optimization.population_size = 250
        optimization.run(25, False)

        optimum = optimization.find_best(optimization.model_dict.models)[0]
        self.assertAlmostEqual(optimum, 0, 1)

if __name__ == '__main__':
    import unittest as ut
    suite = ut.TestSuite()
    result = Agros2DTestResult()
    suite.addTest(ut.TestLoader().loadTestsFromTestCase(TestCrossover))
    suite.addTest(ut.TestLoader().loadTestsFromTestCase(TestMutation))
    suite.addTest(ut.TestLoader().loadTestsFromTestCase(TestSingleCriteriaSelector))
    suite.addTest(ut.TestLoader().loadTestsFromTestCase(TestBoothsFunctionOptimization))
    suite.run(result)
